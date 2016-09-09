/**
 * @file
 *
 * @date 03.09.2016
 * @author marco@kleesiek.com
 * @description
 */

#include <vmcmc/math.hpp>
#include <vmcmc/numeric.hpp>
#include <vmcmc/exception.hpp>

#include <boost/math/distributions/chi_squared.hpp>
#include <boost/math/special_functions/pow.hpp>

using namespace std;
namespace bm = boost::math;

namespace vmcmc
{

namespace math
{

double biVariateNormalPDF(double x1, double x2, double mean1, double mean2, double sigma1, double sigma2, double corr)
{
    if (sigma1 == 0.0)
        return (x1 == mean1) ? bm::pdf( bm::normal(mean2, sigma2), x2 ) : 0.0;

    else if (sigma2 == 0.0)
        return (x2 == mean2) ? bm::pdf( bm::normal(mean1, sigma1), x1 ) : 0.0;

    else if (sigma1 == 0.0 && sigma2 == 0.0)
        throw Exception() << "Cannot calculate a bivariate normal distribution with sigma_1 = 0 and sigma_2 = 0.";

    const double z = pow<2>( (x1-mean1)/sigma1 ) + pow<2>( (x2-mean2)/sigma2 ) - 2.0*corr*(x1-mean1)*(x2-mean2)/sigma1/sigma2;
    return 1.0 / (constants::two_pi*sigma1*sigma2*sqrt(1.0-corr*corr)) * exp(-z/(2.0*(1.0-corr*corr)));
}

double normal1SidedCDF(double nSigmas)
{
    const bm::normal_distribution<double> normal(0.0, 1.0);
    return 1.0 - bm::cdf( bm::complement(normal, nSigmas) ) * 2.0;
}

double normal1SidedQuantile(double prob)
{
    if (prob >= 1.0)
        return numeric::inf();

    const bm::normal_distribution<double> normal(0.0, 1.0);
    return bm::quantile( bm::complement(normal, (1.0-prob)/2.0) );
}

double chiSquareQuantile(double prob, size_t nParams)
{
    if (prob >= 1.0)
        return std::numeric_limits<double>::infinity();

    const bm::chi_squared_distribution<double> chiSquare(nParams);
    return bm::quantile(chiSquare, prob);
}

double chiSquareQuantileFromSigmas(double nSigmas, size_t nParams)
{
    const double prob = normal1SidedCDF(nSigmas);
    return chiSquareQuantile(prob, nParams);
}

double chiSquareCDF(double value, size_t nParams)
{
    const bm::chi_squared_distribution<double> chiSquare(nParams);
    return 1.0 - bm::cdf( bm::complement(chiSquare, value) );
}

double chiSquareToSigmas(double value, size_t nParams)
{
    const bm::chi_squared_distribution<double> chiSquare(nParams);
    const bm::normal_distribution<double> normal(0.0, 1.0);

    const double cProb = bm::cdf( bm::complement(chiSquare, value) );
    return bm::quantile( bm::complement(normal, cProb/2.0) );
}


} /* namespace math */

} /* namespace vmcmc */
