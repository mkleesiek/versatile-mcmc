/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 09.09.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/chain.hpp>
#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(Chain, Basics)
{
    Chain testChain;
    ChainStatistics stats( testChain );

    Sample testSample{ 0.0, 1.0, 2.0 };
    testChain.push_back( testSample );
    testSample = { 1.0, 2.0, 3.0 };
    testChain.push_back( testSample );
    testSample = { 2.0, 3.0, 4.0 };
    testChain.push_back( testSample );

    ASSERT_EQ( 3, stats.NumberOfParams() );

    ASSERT_EQ( stats.GetMean(), (Sample{ 1.0, 2.0, 3.0 }) );
}

TEST(Chain, AcceptanceRate)
{
    Sample testSample( { 0.0, 1.0, 2.0 } );
    Chain testChain;

    ChainStatistics stats( testChain );

    testSample.SetAccepted(true);
    testChain.push_back( testSample );
    testSample.SetAccepted(false);
    testChain.push_back( testSample );

    ASSERT_DOUBLE_EQ( 0.0, stats.GetAccRate() );

    testSample = { 0.0, 3.0, 2.0 };
    testSample.SetAccepted(true);
    testChain.push_back( testSample );
    stats.Reset();
    ASSERT_DOUBLE_EQ( 0.5, stats.GetAccRate() );

    stats.SelectRange(1);
    ASSERT_DOUBLE_EQ( 1.0, stats.GetAccRate() );

    stats.SelectRange(0, 2);
    ASSERT_DOUBLE_EQ( 0.0, stats.GetAccRate() );

    stats.SelectRange(0, -2);
    ASSERT_DOUBLE_EQ( 0.0, stats.GetAccRate() );

    testChain.push_back( testSample );
    stats.SelectRange();
    stats.Reset();
    ASSERT_DOUBLE_EQ( 2.0/3.0, stats.GetAccRate() );
}

TEST(Chain, CovarianceMatrix)
{
    Chain testChain;
    ChainStatistics stats( testChain );

    Sample testSample( { 0.0, 1.0, 2.0 } );
    testChain.push_back( testSample );
    testSample = { 1.0, 2.0, 3.0 };
    testChain.push_back( testSample );
    testSample = { 2.0, 3.0, 4.0 };
    testChain.push_back( testSample );

    MatrixLower cov = stats.GetCovarianceMatrix();

    ASSERT_DOUBLE_EQ( 1.0, cov(0, 0) );
    ASSERT_DOUBLE_EQ( 1.0, cov(1, 1) );
    ASSERT_DOUBLE_EQ( 1.0, cov(2, 2) );
}
