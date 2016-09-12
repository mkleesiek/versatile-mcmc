/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/algorithm.hpp>
#include <vmcmc/exception.hpp>
#include <vmcmc/math.hpp>
#include <vmcmc/io.hpp>
#include <vmcmc/logger.hpp>
#include <vmcmc/stringutils.hpp>

#ifdef USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
using namespace tbb;
#endif // USE_TBB

using namespace std;

namespace vmcmc {

LOG_DEFINE("vmcmc.algorithm");

Algorithm::Algorithm() :
    fTotalLength( 1E6 ),
    fCycleLength( 50 )
{ }

Algorithm::~Algorithm()
{ }

void Algorithm::SetParameterConfig(const ParameterConfig& paramConfig)
{
    fParameterConfig = paramConfig;
}

void Algorithm::Initialize()
{
    if (!(fLikelihood || fNegLogLikelihood))
        throw Exception() << "No target function specified.";

    math::constrain<size_t>(fCycleLength, 1, fTotalLength);

    // TODO: perform consistency checks on the parameter list
}

void Algorithm::Finalize()
{
    fStatistics.ClearChains();

    const size_t nChains = NumberOfChains();
    for (size_t iChain = 0; iChain < nChains; iChain++)
        fStatistics.AddChain( GetChain(iChain) );

    // if TBB is available, calculate diagnostics in parallel
#ifdef USE_TBB
    LOG(Debug, "Precalculating some of the diagnostics ...");

    parallel_for(
        blocked_range<size_t>(0, nChains),
        [&](const blocked_range<size_t>& range) {
            for (size_t iChain = range.begin(); iChain < range.end(); iChain++) {
                ChainStats& stats = fStatistics.GetChainStats( iChain );
                stats.GetMode();
                stats.GetVariance();
                stats.GetAccRate();
                stats.GetAutoCorrelationTime();
            }
        }
    );
#endif // USE_TBB

    // print the diagnostics for each chain to console

    for (size_t iChain = 0; iChain < nChains; iChain++) {

        LOG(Info, "Diagnostics for chain " << iChain << ":");

        ChainStats& stats = fStatistics.GetChainStats( iChain );

        const double accRate = stats.GetAccRate();
        LOG(Info, "  Acceptance Rate: " << accRate);

        const Sample& mode = stats.GetMode();
        LOG(Info, "  Mode: " << mode);

        const double cl = math::normal1SidedCDF( 1.0 );

        for (size_t iParam = 0; iParam < stats.NumberOfParams(); iParam++) {
            auto ci = stats.GetConfidenceInterval(iParam, mode.Values()[iParam], cl );
            LOG(Info, "  68% Confidence interval for parameter " << iParam << ": " << ci);
        }

        Sample& mean = stats.GetMean();
        Evaluate(mean);
        LOG(Info, "  Mean: " << mean);

        for (size_t iParam = 0; iParam < stats.NumberOfParams(); iParam++) {
            double median = stats.GetMedian(iParam);
            LOG(Info, "  Median for parameter " << iParam << ": " << median);
        }

        const Vector& variance = stats.GetVariance();
        LOG(Info, "  Variance: " << variance);

        const Vector& error = stats.GetError();
        LOG(Info, "  Error: " << error);

        const Vector& rms = stats.GetRms();
        LOG(Info, "  RMS: " << rms);

        const Vector& acTime = stats.GetAutoCorrelationTime();
        LOG(Info, "  Autocorrelation time: " << acTime);
    }

    fStatistics.SelectPercentageRange(0.5, 1.0);
    const double R = fStatistics.GetRubinGelman();
    LOG(Info, "Rubin-Gelman diagnostic R: " << R);
}

double Algorithm::EvaluatePrior(const std::vector<double>& paramValues) const
{
    if (!fParameterConfig.IsInsideLimits( paramValues ))
        return 0.0;

    return (fPrior) ? fPrior( paramValues ) : 1.0;
}

double Algorithm::EvaluateLikelihood(const std::vector<double>& paramValues) const
{
    LOG_ASSERT(fLikelihood || fNegLogLikelihood, "No target function specified.");

    return (fLikelihood) ? fLikelihood( paramValues ) : exp( -fNegLogLikelihood( paramValues ) );
}

double Algorithm::EvaluateNegLogLikelihood(const std::vector<double>& paramValues) const
{
    LOG_ASSERT(fLikelihood || fNegLogLikelihood, "No target function specified.");

    return (fNegLogLikelihood) ? fNegLogLikelihood( paramValues ) : -log(fLikelihood( paramValues ));
}

bool Algorithm::Evaluate(Sample& sample) const
{
    LOG_ASSERT(fLikelihood || fNegLogLikelihood, "No target function specified.");

    sample.Reset();

    if (!fParameterConfig.IsInsideLimits( sample.Values() ))
        return false;

    const std::vector<double>& paramValues = sample.Values().data();

    const double prior = (fPrior) ? fPrior( paramValues ) : 1.0;
    if (prior == 0.0)
        return false;

    sample.SetPrior( prior );

    if (fLikelihood) {
        const double likelihood = fLikelihood( paramValues );
        sample.SetLikelihood( likelihood );
        sample.SetNegLogLikelihood( -log(likelihood) );
    }
    else {
        const double negLogLikelihood = fNegLogLikelihood( paramValues );
        sample.SetNegLogLikelihood( negLogLikelihood );
        sample.SetLikelihood( exp(-negLogLikelihood) );
    }

    return true;
}

void Algorithm::Run()
{
    Initialize();

    // Let the derived sampler instance advance the Markov chain for
    // nCycles of fCycleLength plus nRemainingSteps to yield a total chain
    // length of fTotalLength.

    const size_t nCycles = fTotalLength / fCycleLength;
    const size_t nChains = NumberOfChains();

    vector<reference_wrapper<const Chain>> chainRefs;
    for (size_t iChain = 0; iChain < nChains; iChain++)
        chainRefs.emplace_back( GetChain(iChain) );

    // print the starting points
    for (size_t iChain = 0; iChain < nChains; iChain++) {
        const Chain& chain = chainRefs[iChain];
        if (!chain.empty())
            LOG(Info, "Chain " << iChain << " starting point: " << chain.back());
    }

    // length trackers for each chain
    vector<size_t> cChainLengths(nChains, 0);

    // initialize writers
    for (auto& writer : fWriters)
        writer->Initialize( nChains, fParameterConfig );

    // advance the samplers in cycles
    for (size_t iCycle = 0; iCycle <= nCycles; iCycle++) {

        const size_t nSteps = (iCycle < nCycles) ? fCycleLength : fTotalLength % fCycleLength;

        if (nSteps == 0)
            break;

        Advance(nSteps);

        // output new samples and update length counters
        for (size_t iChain = 0; iChain < nChains; iChain++) {
            const Chain& chain = chainRefs[iChain];
            for (auto& writer : fWriters) {
                writer->Write( iChain, chainRefs[iChain], cChainLengths[iChain] );
            }

            cChainLengths[iChain] = chain.size();
        }


        // some intermediate logging (in 5% progress increments)
        if ((iCycle+1) % (nCycles/20) == 0) {
            const size_t iStep = (iCycle+1) * fCycleLength;

            for (size_t iChain = 0; iChain < nChains; iChain++) {
                const Sample& sample = GetChain(iChain).back();

                LOG(Info, "Chain " << iChain << ", step " << iStep << " (" <<
                      ((iCycle+1)*100/nCycles) << "%): " << sample);
            }
        }
    }

    Finalize();

    LOG(Info, "MCMC run finished.");
}

} /* namespace vmcmc */
