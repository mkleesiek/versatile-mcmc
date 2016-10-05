/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/math.hpp>
#include <vmcmc/numeric.hpp>
#include <vmcmc/exception.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;
using namespace vmcmc::math;

TEST(Math, Pow)
{
    ASSERT_DOUBLE_EQ( 8.0, pow<3>(2.0) );
}

TEST(Math, Constrain)
{
    int itest = 15;

    ASSERT_EQ( 15, constrain(itest, 30, 10) );
    ASSERT_EQ( 10, constrain(itest, 1, 10) );
    ASSERT_EQ( 10, itest );

    ASSERT_DOUBLE_EQ( 5.0, constrain(1.0, 5.0, 20.0) );
    ASSERT_DOUBLE_EQ( -6.0, constrain(255.0, -10.0, -6.0) );
}

TEST(Math, NormalPDF)
{
    ASSERT_NEAR( 0.3521, normalPDF(0.5, 0.0, 1.0), 0.001 );

    ASSERT_ANY_THROW( normalPDF(0.0, 0.0, 0.0) );
}

TEST(Math, BiVariateNormalPDF)
{
    ASSERT_NEAR( 0.0885, biVariateNormalPDF(0.5, 0.5, 0.0, 0.0, 1.0, 1.0, -0.75), 0.001 );

    ASSERT_THROW( biVariateNormalPDF(0.0, 0.0, 0.0, 0.0, 0.0, 0.0), Exception );
}

TEST(Math, Normal1SidedCDF)
{
    ASSERT_NEAR( 0.683, normal1SidedCDF(1.0), 0.001 );
    ASSERT_NEAR( 0.9, normal1SidedCDF(1.645), 0.001 );
    ASSERT_NEAR( 0.954, normal1SidedCDF(2.0), 0.001 );

    ASSERT_NEAR( 0.9, normal1SidedCDF(normal1SidedQuantile(0.9) ), 1E-9 );
}

TEST(Math, Normal1SidedQuantile)
{
    ASSERT_NEAR( 1.0, normal1SidedQuantile(0.683), 0.01 );
    ASSERT_NEAR( 1.645, normal1SidedQuantile(0.9), 0.01 );
    ASSERT_NEAR( 2.0, normal1SidedQuantile(0.954), 0.01 );

    ASSERT_NEAR( 1.645, normal1SidedQuantile(normal1SidedCDF(1.645) ), 1E-9 );

    ASSERT_DOUBLE_EQ( numeric::inf(), normal1SidedQuantile(1.1) );
}

TEST(Math, ChiSquareQuantile)
{
    ASSERT_NEAR( 1.0, chiSquareQuantile(normal1SidedCDF(1.0), 1), 0.001 );

    ASSERT_NEAR( 2.70554, chiSquareQuantile(0.9, 1), 0.001 );
    ASSERT_NEAR( 4.60517, chiSquareQuantile(0.9, 2), 0.001 );

    ASSERT_DOUBLE_EQ( numeric::inf(), chiSquareQuantile(1.0, 50) );
}

TEST(Math, ChiSquareQuantileFromSigmas)
{
    ASSERT_NEAR( 1.0, chiSquareQuantileFromSigmas(1.0, 1), 0.001 );
    ASSERT_NEAR( 2.2957, chiSquareQuantileFromSigmas(1.0, 2), 0.001 );

    ASSERT_NEAR( 4.0, chiSquareQuantileFromSigmas(2.0, 1), 0.001 );
    ASSERT_NEAR( 6.18, chiSquareQuantileFromSigmas(2.0, 2), 0.001 );
}

TEST(Math, ChiSquareCDF)
{
    ASSERT_NEAR( 0.9545, chiSquareCDF(4.0, 1), 0.001 );
    ASSERT_NEAR( 0.9545, chiSquareCDF(6.18, 2), 0.001 );
}

TEST(Math, ChiSquareToSigmas)
{
    ASSERT_NEAR( 2.0, chiSquareToSigmas(4.0, 1), 0.001 );
    ASSERT_NEAR( 2.0, chiSquareToSigmas(6.18, 2), 0.001 );
}
