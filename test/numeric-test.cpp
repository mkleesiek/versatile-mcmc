/**
 * @file
 *
 * @date 24.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/numeric.h>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

using namespace vmcmc::math;
using namespace vmcmc::numeric;

TEST(Numeric, Pow) {
    ASSERT_DOUBLE_EQ( 8.0, pow<3>(2.0) );
}

TEST(Numeric, FloatComparisons) {
    ASSERT_TRUE( approxEqual(0.9995, 1.0, 0.001) );
    ASSERT_TRUE( essentEqual(0.9995, 1.0, 0.001) );
}

TEST(Numeric, Constrain) {
    int itest = 15;

    ASSERT_EQ( 15, constrain(itest, 30, 10) );
    ASSERT_EQ( 10, constrain(itest, 1, 10) );
    ASSERT_EQ( 10, itest );

    ASSERT_DOUBLE_EQ( 5.0, constrain(1.0, 5.0, 20.0) );
    ASSERT_DOUBLE_EQ( -6.0, constrain(255.0, -10.0, -6.0) );
}

TEST(Numeric, NumberOfDigits) {
    ASSERT_EQ( 1, numberOfDigits(0) );
    ASSERT_EQ( 1, numberOfDigits(1) );
    ASSERT_EQ( 1, numberOfDigits(9) );
    ASSERT_EQ( 2, numberOfDigits(10) );
    ASSERT_EQ( 2, numberOfDigits(11) );
    ASSERT_EQ( 3, numberOfDigits(123) );
}
