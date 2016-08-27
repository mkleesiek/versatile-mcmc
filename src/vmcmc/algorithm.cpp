/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/algorithm.h>
#include <vmcmc/logger.h>
#include <vmcmc/stringutils.h>
#include <vmcmc/stats.h>
#include <vmcmc/io.h>

//#include <boost/accumulators/accumulators.hpp>
//#include <boost/accumulators/statistics/mean.hpp>
//#include <boost/accumulators/statistics/variance.hpp>
//#include <boost/accumulators/statistics/stats.hpp>

using namespace std;
using namespace boost;

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

bool Algorithm::Initialize()
{
    if (!(fLikelihood || fNegLogLikelihood)) {
        LOG(Error, "No target function specified.");
        return false;
    }

    numeric::constrain<size_t>(fCycleLength, 1, fTotalLength);

    // TODO: perform consistency checks on the parameter list

    return true;
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

    const std::vector<double>& paramValues = sample.Values().data();

    if (!fParameterConfig.IsInsideLimits( sample.Values() ))
        return false;

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
    if (!Initialize()) {
        LOG(Error, "Initialization failed, aborting.");
        return;
    }

    // Let the derived sampler instance advance the Markov chain for
    // nCycles of fCycleLength plus nRemainingSteps to yield a total chain
    // length of fTotalLength

    const size_t nCycles = fTotalLength / fCycleLength;
    const size_t nChains = NChains();

    vector<unique_ptr<Writer>> writers(nChains);
    if (fWriter) {
        for (size_t iChain = 0; iChain < nChains; iChain++) {
            writers[iChain].reset( fWriter->Clone() );
            writers[iChain]->Initialize(iChain, nChains);
        }
    }

    for (size_t iCycle = 0; iCycle < nCycles; iCycle++) {
        Advance(fCycleLength);

        // output
        if (fWriter) {
            for (size_t iChain = 0; iChain < nChains; iChain++) {
                writers[iChain]->Write( GetChain(iChain) );
            }
        }

        // some intermediate logging
        if (iCycle % (nCycles/100) == 0) {
            const size_t iStep = iCycle * fCycleLength;
            for (size_t iChain = 0; iChain < nChains; iChain++) {
                const Sample& sample = GetChain(iChain).back();
                LOG(Debug, "(" << iChain << ") " << iStep << ": " << sample);
            }
        }
    }

    const size_t nRemainingSteps = fTotalLength % fCycleLength;
    if (nRemainingSteps > 0)
        Advance(nRemainingSteps);

    // print some diagnostics for each chain to console

    for (size_t iChain = 0; iChain < NChains(); iChain++) {
        const Chain& chain = GetChain(iChain);

        LOG(Info, "Diagnostics for chain " << iChain << ":");

        const double accRate = stats::accRate(chain);

        LOG(Info, "  Acceptance Rate: " << accRate);
    }

    LOG(Info, "MCMC run finished.");
}

} /* namespace vmcmc */
