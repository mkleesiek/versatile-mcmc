/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 24.07.2016
 * @author marco@kleesiek.com
 *
 * @brief Metropolis-Hastings usage example.
 *
 * A simple, very preliminary example of how the versatile-mcmc
 * library can be used to run a metropolis-hastings sampler on a simple
 * statistical distribution and save the output to an ASCII file.
 */

#include <vmcmc/io.hpp>
#include <vmcmc/logger.hpp>
#include <vmcmc/metropolis.hpp>
#include <vmcmc/math.hpp>
#include <vmcmc/proposal.hpp>
#include <vmcmc/random.hpp>
#include <vmcmc/stringutils.hpp>

LOG_DEFINE("vmcmc.example")

using namespace std;
using namespace vmcmc;

/**
 * The likelihood function (a bivariate normal distribution) for this particular
 * example.
 *
 * Alternatively, the arguments can be encompassed in a vector:
 * @code{.cpp}
 * double targetFunction(const vector<double>& p) {
 *     return math::biVariateNormalPDF(p[0], p[1], 0.0, 0.0, 2.0, 3.0, 0.5);
 * }
 * @endcode
 */
double targetFunction(double x1, double x2) {
    return math::biVariateNormalPDF(x1, x2, 0.0, 0.0, 2.0, 3.0, 0.5);
}

int main(int /*argc*/, char* /*argv*/[]){

    LOG(Info, "Setting up Metropolis-Hastings example ...");

    // choose a non-deterministic seed for random number generator:
    Random::Seed( 0 );

    // setup the parameter configuration
    ParameterConfig paramConfig;
    paramConfig.SetParameter(0, Parameter("x1", 0.0, 1.0) );
    paramConfig.SetParameter(1, Parameter("x2", 0.0, 1.0) );
    paramConfig.SetErrorScaling( 5.0 );

    // instantiate the MCMC sampler
    MetropolisHastings mcmc;
    mcmc.SetParameterConfig(paramConfig);

    // randomize the start points within their specified errors
    mcmc.SetRandomizeStartPoint(true);

    // sample multiple sets of chains in parallel
    mcmc.SetNumberOfChains( 3 );

    // enable parallel tempering by specifying higher reciprocal temperatures
    mcmc.SetBetas( {1.0, 0.1} );

    // set the target likelihood
    mcmc.SetLikelihood<2>( targetFunction );

    // use a Gaussian proposa function
    mcmc.SetProposalFunction<ProposalNormal>();

    // set the total number of steps per chain
    mcmc.SetTotalLength(1E5);

    // define the output methods
    mcmc.AddWriter<TextFileWriter>(".", "vmcmc-example");

    // highly experimential chain visualization
//    mcmc.AddWriter<GnuplotWriter>();

    LOG(Info, "Start sampling ...");

    // execute
    mcmc.Run();

    LOG(Info, "Done.");

    return 0;
}

