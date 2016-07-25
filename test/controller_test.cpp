/*
 * controller_test.cpp
 *
 *  Created on: 24.07.2016
 *      Author: marco@kleesiek.com
 */

#include "fmcmc/controller.h"

#include <gtest/gtest.h>

using namespace fmcmc;

TEST(basic_test, eq_works) {
    ASSERT_EQ(0, 1-1) << "Equality is broken. Mass panic!";
}

TEST(basic_test, neq_works) {
    ASSERT_NE(15, 106) << "Inequal is equal. The foundations of space and time are in jeopardy.";
}
