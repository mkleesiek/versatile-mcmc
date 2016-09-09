/**
 * @file
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/math.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;
using namespace vmcmc::math;

TEST(Math, Pow) {
    ASSERT_DOUBLE_EQ( 8.0, pow<3>(2.0) );
}

TEST(Math, Constrain) {
    int itest = 15;

    ASSERT_EQ( 15, constrain(itest, 30, 10) );
    ASSERT_EQ( 10, constrain(itest, 1, 10) );
    ASSERT_EQ( 10, itest );

    ASSERT_DOUBLE_EQ( 5.0, constrain(1.0, 5.0, 20.0) );
    ASSERT_DOUBLE_EQ( -6.0, constrain(255.0, -10.0, -6.0) );
}
