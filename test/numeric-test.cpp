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

TEST(Numeric, Pow) {
    ASSERT_DOUBLE_EQ( math::pow<3>(2.0), 8.0 );
}

TEST(Numeric, FloatComparisons) {
    ASSERT_TRUE( approxEqual(0.9995, 1.0, 0.001) );
    ASSERT_TRUE( essentEqual(0.9995, 1.0, 0.001) );
}
