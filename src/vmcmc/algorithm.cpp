/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/algorithm.h>
#include <vmcmc/logger.h>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/stats.hpp>

using namespace std;
using namespace boost;
using namespace boost::accumulators;

namespace vmcmc {

LOG_DEFINE("vmcmc.algorithm");

Algorithm::Algorithm() :
    fTotalLength( 1E6 )
{ }

Algorithm::~Algorithm()
{ }

void Algorithm::SetParameterConfig(const ParameterSet& paramConfig)
{
    // TODO: perform consistency checks
    fParameterConfig = paramConfig;
}

bool Algorithm::Initialize()
{
    if (!(fLikelihood || fNegLogLikelihood)) {
        LOG(Error, "No target function specified.");
        return false;
    }

    return true;
}

void Algorithm::Evaluate(const std::vector<double>& paramValues,
        double& likelihood, double& negLogLikelihood, double& prior) const
{
    LOG_ASSERT(fLikelihood || fNegLogLikelihood, "No target function specified.");

    if (fLikelihood) {
        likelihood = fLikelihood( paramValues );
        negLogLikelihood = -log(likelihood);
    }
    else {
        negLogLikelihood = fNegLogLikelihood( paramValues );
        likelihood = exp(-negLogLikelihood);
    }

    prior = (fPrior) ? fPrior( paramValues ) : 1.0;
}

void Algorithm::Evaluate(Sample& sample) const
{
    LOG_ASSERT(fLikelihood || fNegLogLikelihood, "No target function specified.");

    const std::vector<double>& paramValues = sample.Values().data();

    const double likelihood = (fLikelihood) ? fLikelihood(paramValues) : exp(-fNegLogLikelihood(paramValues));
    sample.SetLikelihood( likelihood );

    const double prior = (fPrior) ? fPrior( paramValues ) : 1.0;
    sample.SetPrior( prior );
}

void Algorithm::Run()
{
    if (!Initialize()) {
        LOG(Error, "Initialization failed, aborting.");
        return;
    }

    accumulator_set<double, stats<tag::mean>> accRateAcc;

    for (size_t iStep = 0; iStep < fTotalLength; iStep++) {
        const double accRateStep = Advance();
        accRateAcc( accRateStep );
    }

    const double accRate = mean( accRateAcc );

    LOG(Info, "Acceptance Rate: " << accRate);
}

} /* namespace vmcmc */
