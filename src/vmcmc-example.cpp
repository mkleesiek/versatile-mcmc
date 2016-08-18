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

double normalPdf(double x, double mean, double sigma)
{
    return 1.0 / (constants::root_two_pi * sigma) * exp( - 0.5 * math::pow<2>((x - mean) / sigma) );
}

double biVariateNormalPdf(double x1, double x2, double mean1, double mean2, double sigma1, double sigma2, double corr)
{
    if (sigma1 == 0.0)
        return (x1 == mean1) ? normalPdf(x2, mean2, sigma2) : 0.0;

    if (sigma2 == 0.0)
        return (x2 == mean2) ? normalPdf(x2, mean1, sigma1): 0.0;

    const double z = boost::math::pow<2>( (x1-mean1)/sigma1 ) + boost::math::pow<2>( (x2-mean2)/sigma2 ) - 2.0*corr*(x1-mean1)*(x2-mean2)/sigma1/sigma2;
    return 1.0 / (constants::two_pi*sigma1*sigma2*std::sqrt(1.0-corr*corr)) * std::exp(-z/(2.0*(1.0-corr*corr)));
}

double targetFunction(const vector<double>& params) {
    const double x1 = params[0];
    const double x2 = params[1];
    return biVariateNormalPdf(x1, x2, 0.0, 0.0, 2.0, 3.0, 0.5);
}

int main(int /*argc*/, char* /*argv*/[]){

    Random::Instance().SetSeed(0);

    ParameterList paramConfig;
    paramConfig.SetParameter(0, Parameter("x1", 0.0, 1.0) );
    paramConfig.SetParameter(1, Parameter("x2", 0.0, 1.0) );
    paramConfig.SetErrorScaling( 5.0 );

    MetropolisHastings mcmc;
    mcmc.SetParameterConfig(paramConfig);
    mcmc.SetRandomizeStartPoint(true);
    mcmc.SetBetas( {1.0, 0.1, 0.01, 0.001} );
    mcmc.SetLikelihoodFunction( targetFunction );
    mcmc.SetTotalLength(1E5);

    LOG(Info, "Starting example Metropolis ...");

    mcmc.Run();

    LOG(Info, "Done.");

    return 0;
}

