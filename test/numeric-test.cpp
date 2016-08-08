/*
 * numeric-test.cpp
 *
 *  Created on: 24.07.2016
 *      Author: marco@kleesiek.com
 */

#include <fmcmc/numeric.h>

#include <gtest/gtest.h>

using namespace std;
using namespace fmcmc;

TEST(numeric_test, pow) {
    ASSERT_DOUBLE_EQ( math::pow<3>(2.0), 8.0 );
}

TEST(numeric_test, float_comparisons) {
    ASSERT_TRUE( approxEqual(0.9995, 1.0, 0.001) );
    ASSERT_TRUE( essentEqual(0.9995, 1.0, 0.001) );
}
