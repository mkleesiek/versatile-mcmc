/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <fmcmc/algorithm.h>
#include <fmcmc/logger.h>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/stats.hpp>

using namespace std;
using namespace boost;
using namespace boost::accumulators;

namespace fmcmc {

LOG_DEFINE("fmcmc.algorithm");

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

} /* namespace fmcmc */
