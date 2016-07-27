/*
 * numeric.h
 *
 *  Created on: 27.07.2016
 *      Author: marco@kleesiek.com
 */

#ifndef SRC_FMCMC_NUMERIC_H_
#define SRC_FMCMC_NUMERIC_H_

#include <numeric>
#include <boost/math/special_functions/pow.hpp>

namespace fmcmc {

template <int N, typename T>
inline T pow(const T& base)
{
    return boost::math::pow<N>(base);
}

template <class T>
inline T NaN()
{
    return std::numeric_limits<T>::quiet_NaN();
}

template<class IntegerT>
inline bool isOdd(const IntegerT& v)
{
    return (v % 2 == 1);
}

template<class IntegerT>
inline bool isEven(const IntegerT& v)
{
    return (v % 2 == 0);
}

template<class T>
inline void limit(T& input, const T& min, const T& max)
{
    if (min > max)
        return;
    else if (input < min)
        input = min;
    else if (input > max)
        input = max;
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

} /* namespace fmcmc */

#endif /* SRC_FMCMC_NUMERIC_H_ */
