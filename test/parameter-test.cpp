/**
 * @file
 *
 * @date 26.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/parameter.hpp>
#include <vmcmc/exception.hpp>
#include <vmcmc/stringutils.hpp>

#include <boost/numeric/ublas/io.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(Parameter, Limits)
{
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

TEST(ParameterConfig, Correlations)
{
    ParameterConfig paramConfig;
    paramConfig.SetParameter(0, Parameter("p1", 0.0, 1.0) );
    paramConfig.SetParameter(1, Parameter("p2", -1.0, 2.0) );
    paramConfig.SetParameter(2, Parameter("p3", +1.0, 1.5) );

    paramConfig.SetCorrelation(1, 0, 0.7);
    paramConfig.SetCorrelation(0, 1, 0.7);
    paramConfig.SetCorrelation(0, 0, 1.0);

    paramConfig.SetCorrelation(2, 1, -0.5);

    auto corMatrix = paramConfig.GetCorrelationMatrix();
    auto covMatrix = paramConfig.GetCovarianceMatrix();
    auto cholDecomp = paramConfig.GetCholeskyDecomp();

    ASSERT_DOUBLE_EQ(corMatrix(1, 0), 0.7);
    ASSERT_DOUBLE_EQ(corMatrix(2, 1), -0.5);

    ASSERT_NEAR(cholDecomp(1, 1), 1.42829, 1E-4);
    ASSERT_NEAR(cholDecomp(2, 1), -1.05021, 1E-4);

    // test IO
    cout << corMatrix << endl;
    cout << covMatrix << endl;
    cout << cholDecomp << endl;
}
