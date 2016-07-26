/*
 * logging_test.cpp
 *
 *  Created on: 24.07.2016
 *      Author: marco@kleesiek.com
 */

#include <gtest/gtest.h>
#include "../src/fmcmc/logger.h"

LOG_DEFINE("fmcmc.default-logger");
LOG_DEFINE(myLogger1, "fmcmc.logger1");
LOG_DEFINE(myLogger2, "fmcmc.logger2");

TEST(logging_test, basics) {
//    ASSERT_EQ(0, 1-1) << "Equality is broken. Mass panic!";
    LOG(Debug, "This is a debug message.");
}
