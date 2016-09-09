/**
 * @file
 *
 * @date 24.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/numeric.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

using namespace vmcmc::numeric;

TEST(Numeric, FloatComparisons) {
    ASSERT_TRUE( approxEqual(0.9995, 1.0, 0.001) );
    ASSERT_TRUE( essentEqual(0.9995, 1.0, 0.001) );
}

TEST(Numeric, NumberOfDigits) {
    ASSERT_EQ( 1, numberOfDigits(0) );
    ASSERT_EQ( 1, numberOfDigits(1) );
    ASSERT_EQ( 1, numberOfDigits(9) );
    ASSERT_EQ( 2, numberOfDigits(10) );
    ASSERT_EQ( 2, numberOfDigits(11) );
    ASSERT_EQ( 3, numberOfDigits(123) );
}
