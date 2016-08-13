/**
 * @file
 *
 * @date 24.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/logger.h>
#include <vmcmc/random.h>
#include <vmcmc/stringutils.h>
#include <vmcmc/metropolis.h>
#include <vmcmc/numeric.h>

LOG_DEFINE("vmcmc.example")

using namespace std;
using namespace vmcmc;

double function(const vector<double>& params) {
    const double x = params[0];
    constexpr double mean = 0.0;
    constexpr double sigma = 1.0;
    return 1.0 / (constants::root_two_pi * sigma) * exp( - 0.5 * math::pow<2>((x - mean) / sigma) );
}

int main(int argc, char* argv[]){

    Random::Instance().SetSeed(0);

    ParameterSet paramConfig;
    paramConfig.SetParameter(0, Parameter("mean", 0.0, 1.0) );

    MetropolisHastings mcmc;
    mcmc.SetParameterConfig(paramConfig);
    mcmc.SetBetas( {1.0} );

    LOG(Info, "Starting example Metropolis ...");

    mcmc.Run();

    return 0;
}

