/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 26.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/parameter.hpp>
#include <vmcmc/exception.hpp>
#include <vmcmc/stringutils.hpp>

#include <boost/numeric/ublas/io.hpp>
#include <iostream>

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
    ASSERT_THROW( Parameter("bad parameter", 0.0, 1.0, 1.0, -1.0), Exception );
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
    ASSERT_DOUBLE_EQ(-0.5, paramConfig.GetCorrelation(2, 1));

    auto corMatrix = paramConfig.GetCorrelationMatrix();
    auto covMatrix = paramConfig.GetCovarianceMatrix();
    auto cholDecomp = paramConfig.GetCholeskyDecomp();

    ASSERT_DOUBLE_EQ(0.7, corMatrix(1, 0));
    ASSERT_DOUBLE_EQ(-0.5, corMatrix(2, 1));

    ASSERT_NEAR(1.42829, cholDecomp(1, 1), 1E-4);
    ASSERT_NEAR(-1.05021, cholDecomp(2, 1), 1E-4);

    // test IO
    ostringstream strm;
    strm << corMatrix;
    ASSERT_EQ( "[3,3]((1,0,0),(0.7,1,0),(0,-0.5,1))", strm.str() );

    strm.str("");
    strm << covMatrix;
    ASSERT_EQ( "[3,3]((1,0,0),(1.4,4,0),(0,-1.5,2.25))", strm.str() );

    strm.str("");
    strm << cholDecomp;
    ASSERT_EQ( "[3,3]((1,0,0),(1.4,1.42829,0),(0,-1.05021,1.07101))", strm.str() );

    // check graceful failure of cholesky decomposition:

    paramConfig.SetCorrelation(0, 1, -1.0);
    paramConfig.SetCorrelation(1, 2, 1.0);

    MatrixLower exp(3, 3);
    exp(0, 0) = 1.0;
    exp(1, 1) = 2.0;
    exp(2, 2) = 1.5;

    ASSERT_EQ( exp, paramConfig.GetCholeskyDecomp() );
}

TEST(ParameterConfig, RelativeError)
{
    ParameterConfig paramConfig;
    paramConfig.SetParameter(0, Parameter("p1", 0.0, 1.0) );
    paramConfig.SetParameter(1, Parameter("p2", -1.0, 2.0) );
    paramConfig.SetParameter(2, Parameter("p3", +1.0, 1.5) );

    paramConfig.GetParameter(1).SetRelativeError( 5.0 );

    ASSERT_DOUBLE_EQ( 5.0, paramConfig.GetParameter(1).GetAbsoluteError() );

    vector<double> expErrs{ 1.0, 5.0, 1.5 };
    ASSERT_TRUE( Vector(expErrs) == paramConfig.GetErrors() );
}
