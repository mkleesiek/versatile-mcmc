/**
 * @file
 *
 * @date 24.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/logger.hpp>

#include <gtest/gtest.h>

LOG_DEFINE("vmcmc.default-logger");
LOG_DEFINE(myLogger1, "vmcmc.logger1");
LOG_DEFINE(myLogger2, "vmcmc.logger2");

using namespace std;

TEST(Logger, Basics) {

    LOG(Trace, "Some arbitrary message with arbitrary contents: " << '#' << "\t" << 1E-7);

    testing::internal::CaptureStdout();
    LOG(myLogger1, Debug, "This is a debug message.");
    string output = testing::internal::GetCapturedStdout();

#ifdef NDEBUG
    ASSERT_EQ( output.find("debug message"), string::npos );
#else
    ASSERT_NE( output.find("debug message"), string::npos );
#endif

    testing::internal::CaptureStderr();
    LOG(myLogger2, Error, "This is an error message.");
    output = testing::internal::GetCapturedStderr();
    ASSERT_TRUE( output.find("error message") != string::npos );
}
