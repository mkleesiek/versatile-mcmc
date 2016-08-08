/*
 * parameter-test.cpp
 *
 *  Created on: 26.07.2016
 *      Author: marco@kleesiek.com
 */

#include <fmcmc/parameter.h>
#include <fmcmc/exception.h>

#include <gtest/gtest.h>

using namespace std;
using namespace fmcmc;

TEST(parameter_test, check_limits) {
    Parameter p("test parameter", 0.0, 1.0, -1.0, +1.0);

    ASSERT_FALSE( p.IsFixed() );

    ASSERT_FALSE( p.IsInsideLimits(-1.1) );
    ASSERT_FALSE( p.IsInsideLimits(+1.1) );

    ASSERT_TRUE( p.IsInsideLimits(-1.0) );
    ASSERT_TRUE( p.IsInsideLimits(+1.0) );

    double lv = 3.0;
    p.ConstrainToLimits(lv);
    ASSERT_DOUBLE_EQ( lv, 1.0 );

    ASSERT_THROW( Parameter("bad parameter", 5.0, 0.0, 2.0, 4.0), Exception );
}
