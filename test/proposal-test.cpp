/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/proposal.hpp>
#include <vmcmc/stringutils.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(ProposalNormal, Clone)
{
    ParameterConfig pc;
    pc.SetParameter(0, "p1", 0.0, 1.0);
    pc.SetParameter(1, "p2", 0.0, 1.0);
    pc.SetCorrelation(0, 1, -0.8);

    ProposalNormal prop;
    prop.UpdateParameterConfig( pc );
    MatrixLower cholesky = prop.GetCholeskyDecomp();

    unique_ptr<ProposalNormal> prop2( prop.Clone() );

    ASSERT_EQ( cholesky, prop2->GetCholeskyDecomp() );
}

TEST(ProposalStudentT, Clone)
{
    ParameterConfig pc;
    pc.SetParameter(0, "p1", 0.0, 1.0);
    pc.SetParameter(1, "p2", 0.0, 1.0);
    pc.SetCorrelation(0, 1, -0.8);

    ProposalStudentT prop;
    prop.UpdateParameterConfig( pc );
    MatrixLower cholesky = prop.GetCholeskyDecomp();

    unique_ptr<ProposalStudentT> prop2( prop.Clone() );

    ASSERT_EQ( cholesky, prop2->GetCholeskyDecomp() );
}

TEST(ProposalNormal, Transition)
{
    Random::Instance().Seed(123);

    ParameterConfig pc;
    pc.SetParameter(0, "p1", 0.0, 1.0);
    pc.SetParameter(1, "p2", 0.0, 1.0);
    pc.SetCorrelation(0, 1, 0.8);

    ProposalNormal prop;
    prop.UpdateParameterConfig( pc );
    Sample v1{ 0.0, 0.0 };
    Sample v2( 2 );
    prop.Transition(v1, v2);

    double exp[] = { 1.36504, 0.181683 };
    for (size_t i = 0; i < v2.size(); i++)
        ASSERT_NEAR( exp[i], v2[i], 0.001 );
}

TEST(ProposalNormal, UpdateParamConfig)
{
    Random::Instance().Seed(123);

    ParameterConfig pc;
    pc.SetParameter(0, "p1", 0.0, 1.0);
    pc.SetParameter(1, "p2", 0.0, 1.0);
    pc.SetCorrelation(0, 1, 0.8);

    ProposalNormal prop;
    prop.UpdateParameterConfig( pc );
    MatrixLower cholesky = prop.GetCholeskyDecomp();

    Random::Instance().Seed(123);
    prop.UpdateParameterConfig( pc );

    ASSERT_EQ( cholesky, prop.GetCholeskyDecomp() );

}

TEST(ProposalStudentT, Transition)
{
    Random::Instance().Seed(123);

    ParameterConfig pc;
    pc.SetParameter(0, "p1", 0.0, 1.0);
    pc.SetParameter(1, "p2", 0.0, 1.0);
    pc.SetCorrelation(0, 1, -0.8);

    ProposalStudentT prop;
    prop.UpdateParameterConfig( pc );
    Sample v1( { 0.0, 0.0 } );
    Sample v2( 2 );
    prop.Transition(v1, v2);

    double exp1[] = { 1.90133, 0.03373 };
    for (size_t i = 0; i < v2.size(); i++)
        ASSERT_NEAR( exp1[i], v2[i], 0.001 );

    prop.SetDOF(2.0);
    prop.Transition(v1, v2);

    double exp2[] = { -1.75956, 1.27399 };
    for (size_t i = 0; i < v2.size(); i++)
        ASSERT_NEAR( exp2[i], v2[i], 0.001 );
}
