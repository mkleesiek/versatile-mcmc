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

TEST(Numeric, Pow) {
    ASSERT_DOUBLE_EQ( math::pow<3>(2.0), 8.0 );
}

TEST(Numeric, FloatComparisons) {
    ASSERT_TRUE( approxEqual(0.9995, 1.0, 0.001) );
    ASSERT_TRUE( essentEqual(0.9995, 1.0, 0.001) );
}
