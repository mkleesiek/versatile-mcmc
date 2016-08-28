/**
 * @file
 *
 * @date 27.07.2016
 * @author marco@kleesiek.com
 */

#ifndef FMCMC_NUMERIC_H_
#define FMCMC_NUMERIC_H_

#include <numeric>
#include <type_traits>

#include <boost/math/special_functions/pow.hpp>
#include <boost/math/constants/constants.hpp>

namespace vmcmc {

namespace math = boost::math;
namespace constants = boost::math::double_constants;

namespace numeric {

template <class T = double>
inline T NaN()
{
    return std::numeric_limits<T>::quiet_NaN();
}

template <class T = double>
inline T inf()
{
    return std::numeric_limits<T>::infinity();
}

template<class T,
class = typename std::enable_if<std::is_integral<T>::value>::type>
inline bool isOdd(const T& v)
{
    return (v % 2 == 1);
}

template<class T,
class = typename std::enable_if<std::is_integral<T>::value>::type>
inline bool isEven(const T& v)
{
    return (v % 2 == 0);
}

template<class T,
class = typename std::enable_if<std::is_integral<T>::value>::type>
inline int numberOfDigits(T number)
{
    if (number == 0)
        return 1;

    int nDigits = 0;
    while (number != 0) {
        nDigits++;
        number /= 10;
    }

    return nDigits;
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

/**
 * Check whether two float values differ by a given epsilon.
 * @param a
 * @param b
 * @param epsilon
 * @return <code>return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);</code>
 */
template<class T>
inline bool approxEqual(T a, T b, T epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

template<class T>
inline bool approxLessOrEqual(T a, T b, T epsilon)
{
    return a < b || approximatelyEqual(a, b, epsilon);
}

template<class T>
inline bool approxGreaterOrEqual(T a, T b, T epsilon)
{
    return a > b || approximatelyEqual(a, b, epsilon);
}

/**
 * Check whether two float values differ by a given epsilon.
 * @param a
 * @param b
 * @param epsilon
 * @return <code>return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);</code>
 */
template<class T>
inline bool essentEqual(T a, T b, T epsilon)
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

template<class T>
inline bool essentLessOrEqual(T a, T b, T epsilon)
{
    return a < b || essentiallyEqual(a, b, epsilon);
}

template<class T>
inline bool essentGreaterOrEqual(T a, T b, T epsilon)
{
    return a > b || essentiallyEqual(a, b, epsilon);
}

} /* namespace numeric */

} /* namespace vmcmc */

#endif /* FMCMC_NUMERIC_H_ */
