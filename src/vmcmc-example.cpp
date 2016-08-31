/**
 * @file
 *
 * @date 24.07.2016
 * @author marco@kleesiek.com
 *
 * @description A simple, very preliminary example of how the v-mcmc
 * library can be used to run a metropolis-hastings sampler on a simple
 * statistical distribution and save the output to an ASCII file.
 */

#include <vmcmc/logger.h>
#include <vmcmc/random.h>
#include <vmcmc/stringutils.h>
#include <vmcmc/metropolis.h>
#include <vmcmc/proposal.h>
#include <vmcmc/numeric.h>
#include <vmcmc/io.h>

LOG_DEFINE("vmcmc.example")

using namespace std;
using namespace vmcmc;

/**
 * A standard normal (Gaussian) distribution.
 * @param x the function argument
 * @param mean the mean of the distribution.
 * @param sigma the standard deviation.
 * @return
 */
double normalPdf(double x, double mean, double sigma)
{
    return 1.0 / (constants::root_two_pi * sigma) * exp( - 0.5 * math::pow<2>((x - mean) / sigma) );
}

/**
 * A 2-dimensional normal distribution.
 * @param x1 function argument #1
 * @param x2 function argument #2
 * @param mean1 distribution mean in dim. 1
 * @param mean2 distribution mean in dim. 2
 * @param sigma1 standard deviation in dim. 1
 * @param sigma2 standard deviation in dim. 2
 * @param corr correlation between x1 and x2
 * @return
 */
double biVariateNormalPdf(double x1, double x2, double mean1, double mean2, double sigma1, double sigma2, double corr)
{
    if (sigma1 == 0.0)
        return (x1 == mean1) ? normalPdf(x2, mean2, sigma2) : 0.0;

    if (sigma2 == 0.0)
        return (x2 == mean2) ? normalPdf(x2, mean1, sigma1): 0.0;

    const double z = boost::math::pow<2>( (x1-mean1)/sigma1 ) + boost::math::pow<2>( (x2-mean2)/sigma2 ) - 2.0*corr*(x1-mean1)*(x2-mean2)/sigma1/sigma2;
    return 1.0 / (constants::two_pi*sigma1*sigma2*std::sqrt(1.0-corr*corr)) * std::exp(-z/(2.0*(1.0-corr*corr)));
}

/**
 * The likelihood function (a bivariate normal distribution) for this particular
 * example run.
 * @param params The first two vector elements are used as function parameters.
 * @return
 */
double targetFunction(const vector<double>& params) {
    const double x1 = params[0];
    const double x2 = params[1];
    return biVariateNormalPdf(x1, x2, 0.0, 0.0, 2.0, 3.0, 0.5);
}

int main(int /*argc*/, char* /*argv*/[]){

    // choose a non-deterministic seed for random number generator:
    Random::Seed(0);

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
    mcmc.SetLikelihoodFunction( targetFunction );

    // use a Gaussian proposa function
    mcmc.SetProposalFunction<ProposalNormal>();

    // set the total number of steps per chain
    mcmc.SetTotalLength(1E5);

    // define the output method
    mcmc.SetWriter<TextFileWriter>(".", "vmcmc-example");

    LOG(Info, "Starting example Metropolis ...");

    // execute
    mcmc.Run();

    // TODO: print diagnostics

    LOG(Info, "Done.");

    return 0;
}

