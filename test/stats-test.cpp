/**
 * @file
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/stats.h>
#include <vmcmc/sample.h>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(Statistics, AcceptanceRate)
{
    Chain testChain;
    testChain.push_back( Sample( { 0.0, 1.0, 2.0 } ) );
    testChain.push_back( Sample( { 0.0, 1.0, 2.0 } ) );

    ASSERT_DOUBLE_EQ( 0.0, stats::accRate(testChain) );

    testChain.push_back( Sample( { 0.0, 3.0, 2.0 } ) );

    ASSERT_DOUBLE_EQ( 0.5, stats::accRate(testChain) );
    ASSERT_DOUBLE_EQ( 1.0, stats::accRate(testChain, 1) );
    ASSERT_DOUBLE_EQ( 0.0, stats::accRate(testChain, 0, 2) );
    ASSERT_DOUBLE_EQ( 0.0, stats::accRate(testChain, 0, -2) );

    testChain.push_back( Sample( { 0.0, 3.0, 2.0 } ) );

    ASSERT_DOUBLE_EQ( 1.0/3.0, stats::accRate(testChain) );
}
