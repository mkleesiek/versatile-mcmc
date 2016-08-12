/*
 * logging-test.cpp
 *
 *  Created on: 24.07.2016
 *      Author: marco@kleesiek.com
 */

#include <fmcmc/logger.h>

#include <gtest/gtest.h>

LOG_DEFINE("fmcmc.default-logger");
LOG_DEFINE(myLogger1, "fmcmc.logger1");
LOG_DEFINE(myLogger2, "fmcmc.logger2");

using namespace std;

TEST(Logger, Basics) {

    LOG(Trace, "Some arbitrary message with arbitrary contents: " << '#' << "\t" << 1E-7);

    testing::internal::CaptureStdout();
    LOG(myLogger1, Debug, "This is a debug message.");
    string output = testing::internal::GetCapturedStdout();

#ifdef DEBUG
    ASSERT_NE( output.find("debug message"), string::npos );
#else
    ASSERT_EQ( output.find("debug message"), string::npos );
#endif

    testing::internal::CaptureStderr();
    LOG(myLogger2, Error, "This is an error message.");
    output = testing::internal::GetCapturedStderr();
    ASSERT_TRUE( output.find("error message") != string::npos );
}
