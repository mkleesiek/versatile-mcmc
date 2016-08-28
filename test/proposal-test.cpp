/**
 * @file
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/proposal.h>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(ProposalNormal, Transition) {
    ProposalNormal prop;
    prop.UpdateParameterConfig( ParameterConfig(2) );
    Sample v1( { 0.0, 0.0 } );
    Sample v2( 2 );
    prop.Transition(v1, v2);
}
