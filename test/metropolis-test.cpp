/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/metropolis.hpp>
#include <vmcmc/math.hpp>
#include <vmcmc/stringutils.hpp>
#include <vmcmc/random.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(Metropolis, SetLikelihood)
{
    MetropolisHastings mcmc;

    mcmc.SetNegLogLikelihood<1>( [](double p1) {
        return math::pow<2>( p1 );
    } );

    ASSERT_DOUBLE_EQ( 0.0, mcmc.EvaluateNegLogLikelihood( {0.0} ) );

    mcmc.SetNegLogLikelihood<2>( [](double p1, double p2) {
        return math::pow<2>( p1*p2 );
    } );

    ASSERT_DOUBLE_EQ( 4.0, mcmc.EvaluateNegLogLikelihood( {1.0, 2.0} ) );

    mcmc.SetNegLogLikelihood( [](const vector<double>& p) {
        return math::pow<2>( p[0]*p[1] );
    } );

    ASSERT_DOUBLE_EQ( 4.0, mcmc.EvaluateNegLogLikelihood( {1.0, 2.0} ) );

    mcmc.SetLikelihood<1>( bind( math::normalPDF, placeholders::_1, 0.0, 1.0) );

    ASSERT_NEAR( 0.39894, mcmc.EvaluateLikelihood( {0.0} ), 0.001 );

    mcmc.SetParameterConfig( ParameterConfig(2) );
    mcmc.SetPrior( [](const vector<double>& p) {
        return 0.5;
    } );

    ASSERT_DOUBLE_EQ( 0.5, mcmc.EvaluatePrior( {0.0, 0.0} ) );
}

TEST(Metropolis, CalculateMHRatio)
{
    MetropolisHastings mcmc;

    ParameterConfig pList;
    pList.SetParameter( 0, Parameter("test1", 0.0, 1.0) );
    pList.SetParameter( 1, Parameter("test2", 0.0, 1.0) );

    mcmc.SetParameterConfig( pList );
    mcmc.SetNegLogLikelihood( [](const std::vector<double>& params) {
        return math::pow<2>( params[0] ) + math::pow<2>( params[1] );
    } );

    Sample s1( { 0.0, 0.0 } );
    Sample s2( { 1.0, 0.0 } );

    mcmc.Evaluate( s1 );
    mcmc.Evaluate( s2 );

    ASSERT_DOUBLE_EQ( 0.0, s1.GetNegLogLikelihood() );
    ASSERT_DOUBLE_EQ( 1.0, s2.GetNegLogLikelihood() );

    double mhRatio = MetropolisHastings::CalculateMHRatio(s1, s2);
    ASSERT_DOUBLE_EQ(0.36787944117144233, mhRatio);

    mhRatio = MetropolisHastings::CalculateMHRatio(s2, s1);
    ASSERT_DOUBLE_EQ(1.0, mhRatio);
}

TEST(Metropolis, Run)
{
    // fix random seed and disable multi-threading to guarantee deterministic
    // behavior

    Random::Instance().Seed(123);

    MetropolisHastings mcmc;
    mcmc.SetMultiThreading(false);

    ParameterConfig pList;
    pList.SetParameter( 0, Parameter("test1", 0.0, 1.0) );
    pList.SetParameter( 1, Parameter("test2", 0.0, 1.0) );
    pList.SetErrorScaling( 2.0 );

    mcmc.SetParameterConfig( pList );
    mcmc.SetNegLogLikelihood( [](const std::vector<double>& params) {
        return 0.5 * ( math::pow<2>( params[0] ) + math::pow<2>( params[1] ) );
    } );

    mcmc.SetNumberOfChains(2);
    mcmc.SetRandomizeStartPoint(false);
    mcmc.SetTotalLength(1E3);
    mcmc.SetBetas( {1.0, 0.1, -1.0, 0.0, 3.0} );

    ASSERT_EQ( vector<double>({1.0, 0.1}), mcmc.GetBetas() );
    ASSERT_EQ( 1000, mcmc.GetTotalLength() );

    mcmc.Run();

    ASSERT_EQ( 2, mcmc.NumberOfChains() );
    ASSERT_EQ( 1001, mcmc.GetChain(0).size() );
    ASSERT_EQ( 1001, mcmc.GetChain(1).size() );

    Sample startPoint{ 0.0, 0.0 };
    mcmc.Evaluate(startPoint);

    ChainSetStatistics stats = mcmc.GetStatistics();

    ASSERT_EQ( startPoint, mcmc.GetChain(0).front() );
    ASSERT_EQ( startPoint, mcmc.GetChain(1).front() );
    ASSERT_EQ( 1.0, mcmc.GetChain(0).front().GetLikelihood() );
    ASSERT_EQ( 0.0, mcmc.GetChain(0).front().GetNegLogLikelihood() );
    ASSERT_EQ( 1.0, mcmc.GetChain(0).front().GetPrior() );

    ASSERT_NEAR( 0.25, stats.GetChainStats(0).GetAccRate(), 0.01 );
    ASSERT_NEAR( 0.32, stats.GetChainStats(1).GetAccRate(), 0.01 );
    ASSERT_NEAR( 0.29, mcmc.GetSwapAcceptanceRate(0), 0.01 );

    ASSERT_NEAR( 0.0, stats.GetChainStats(0).GetMean()[0], 0.25 );
    ASSERT_NEAR( 1.0, stats.GetChainStats(0).GetError()[0], 0.25 );
    ASSERT_NEAR( 0.0, stats.GetChainStats(0).GetMode()[0], 0.25 );
    ASSERT_NEAR( 0.0, stats.GetChainStats(0).GetMedian(0), 0.25 );
}
