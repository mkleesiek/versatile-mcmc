/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 27.07.2016
 * @author marco@kleesiek.com
 *
 * @brief Numeric utility functions.
 */

#ifndef VMCMC_NUMERIC_H_
#define VMCMC_NUMERIC_H_

#include <cmath>
#include <limits>
#include <type_traits>

namespace vmcmc {

namespace numeric {

template <typename T = double>
inline T NaN()
{
    return std::numeric_limits<T>::quiet_NaN();
}

template <typename T = double>
inline T inf()
{
    return std::numeric_limits<T>::infinity();
}

template <typename T,
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

/**
 * Check whether two float values differ by a given epsilon.
 * @param a
 * @param b
 * @param epsilon
 * @return <code>return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);</code>
 */
template <typename T>
inline bool approxEqual(T a, T b, T epsilon)
{
    return std::abs(a - b) <= ( (std::abs(a) < std::abs(b) ? std::abs(b) : std::abs(a)) * epsilon);
}

template <typename T>
inline bool approxLessOrEqual(T a, T b, T epsilon)
{
    return a < b || approximatelyEqual(a, b, epsilon);
}

template <typename T>
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
template <typename T>
inline bool essentEqual(T a, T b, T epsilon)
{
    return std::abs(a - b) <= ( (std::abs(a) > std::abs(b) ? std::abs(b) : std::abs(a)) * epsilon);
}

template <typename T>
inline bool essentLessOrEqual(T a, T b, T epsilon)
{
    return a < b || essentiallyEqual(a, b, epsilon);
}

template <typename T>
inline bool essentGreaterOrEqual(T a, T b, T epsilon)
{
    return a > b || essentiallyEqual(a, b, epsilon);
}

} /* namespace numeric */

} /* namespace vmcmc */

#endif /* VMCMC_NUMERIC_H_ */
