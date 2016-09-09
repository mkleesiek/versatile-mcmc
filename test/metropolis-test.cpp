/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/metropolis.hpp>
#include <vmcmc/math.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(Metropolis, CalculateMHRatio) {
    MetropolisHastings mcmc;
    ParameterConfig pList;
    pList.SetParameter( 0, Parameter("test", 0.0, 1.0) );
    mcmc.SetParameterConfig( pList );
    mcmc.SetNegLogLikelihoodFunction( [](const std::vector<double>& params) {
        return math::pow<2>( params[0] );
    } );

    Sample s1( { 0.0 } );
    Sample s2( { 1.0 } );

    mcmc.Evaluate( s1 );
    mcmc.Evaluate( s2 );

    double mhRatio = MetropolisHastings::CalculateMHRatio(s1, s2);
    ASSERT_DOUBLE_EQ(0.36787944117144233, mhRatio);

    mhRatio = MetropolisHastings::CalculateMHRatio(s2, s1);
    ASSERT_DOUBLE_EQ(1.0, mhRatio);
}

