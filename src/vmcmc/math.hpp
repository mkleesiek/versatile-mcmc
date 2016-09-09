/**
 * @file
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 */

#ifndef VMCMC_MATH_H_
#define VMCMC_MATH_H_

#include <cmath>
#include <type_traits>

#include <boost/math/special_functions/pow.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/math/distributions/normal.hpp>

namespace vmcmc
{

namespace constants = boost::math::double_constants;

namespace math
{

using boost::math::pow;

// DECLARATIONS

template<class T,
class = typename std::enable_if<std::is_integral<T>::value>::type>
bool isOdd(const T& v);

template<class T,
class = typename std::enable_if<std::is_integral<T>::value>::type>
bool isEven(const T& v);

template<class T>
inline T& constrain(T& input, const T& min, const T& max);

template<class T>
inline T constrain(T&& input, const T& min, const T& max);

/**
 * A standard normal (Gaussian) distribution.
 * @param x the function argument
 * @param mean the mean of the distribution.
 * @param sigma the standard deviation.
 * @return
 */
double normalPDF(double x, double mean = 0.0, double sigma = 1.0);

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
double biVariateNormalPDF(double x1, double x2, double mean1 = 0.0, double mean2 = 0.0, double sigma1 = 1.0, double sigma2 = 1.0, double corr = 0.0);

double normal1SidedCDF(double nSigmas = 1.0);
double normal1SidedQuantile(double prob = 0.682689);

double chiSquareQuantile(double prob, size_t nParams);
double chiSquareQuantileFromSigmas(double nSigmas, size_t nParams);

double chiSquareCDF(double value, size_t nParams);
double chiSquareToSigmas(double value, size_t nParams);


// DEFINITIONS

inline double normalPDF(double x, double mean, double sigma)
{
    return boost::math::pdf( boost::math::normal(mean, sigma), x );
}

template<class T, class>
inline bool isOdd(const T& v)
{
    return (v % 2 == 1);
}

template<class T, class>
inline bool isEven(const T& v)
{
    return (v % 2 == 0);
}

template<class T>
inline T& constrain(T& input, const T& min, const T& max)
{
    if (min > max)
        return input;

    if (input < min)
        input = min;
    else if (input > max)
        input = max;

    return input;
}

template<class T>
inline T constrain(T&& input, const T& min, const T& max)
{
    T result = input;
    constrain(result, min, max);
    return result;
}


} /* namespace math */

} /* namespace vmcmc */

#endif /* VMCMC_MATH_H_ */
