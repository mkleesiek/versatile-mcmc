/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/algorithm.h>
#include <vmcmc/exception.h>
#include <vmcmc/logger.h>
#include <vmcmc/stringutils.h>
#include <vmcmc/stats.h>
#include <vmcmc/io.h>

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

    numeric::constrain<size_t>(fCycleLength, 1, fTotalLength);

    // TODO: perform consistency checks on the parameter list
}

void Algorithm::Finalize()
{
    // print some diagnostics for each chain to console

    for (size_t iChain = 0; iChain < NumberOfChains(); iChain++) {
        const Chain& chain = GetChain(iChain);

        LOG(Info, "Diagnostics for chain " << iChain << ":");

        const double accRate = stats::accRate(chain);

        LOG(Info, "  Acceptance Rate: " << accRate);
    }
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

    // print the starting points
    for (size_t iChain = 0; iChain < nChains; iChain++) {
        const auto& chain = GetChain(iChain);
        if (!chain.empty())
            LOG(Info, "Chain " << iChain << " startin point: " << chain.back());
    }

    // configure writers
    vector<unique_ptr<Writer>> writers(nChains);
    if (fWriter) {
        for (size_t iChain = 0; iChain < nChains; iChain++) {
            writers[iChain].reset( fWriter->Clone() );
            writers[iChain]->Initialize(iChain, nChains);
        }
    }

    // advance the samplers in cycles
    for (size_t iCycle = 0; iCycle < nCycles; iCycle++) {
        Advance(fCycleLength);

        // output
        if (fWriter) {
            for (size_t iChain = 0; iChain < nChains; iChain++) {
                writers[iChain]->Write( GetChain(iChain) );
            }
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

    const size_t nRemainingSteps = fTotalLength % fCycleLength;
    if (nRemainingSteps > 0)
        Advance(nRemainingSteps);

    Finalize();

    LOG(Info, "MCMC run finished.");
}

} /* namespace vmcmc */
