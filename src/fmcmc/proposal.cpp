/*
 * Proposal.cpp
 *
 *  Created on: 31.07.2016
 *      Author: marco@kleesiek.com
 */

#include <fmcmc/proposal.h>
#include <fmcmc/random.h>

namespace fmcmc
{

Proposal::Proposal()
{ }

Proposal::~Proposal()
{ }

ProposalGaussian::ProposalGaussian() :
    fCholeskyDecomp( ublas::identity_matrix<double>() )
{ }

ProposalGaussian::~ProposalGaussian()
{ }

double ProposalGaussian::Transition(const ublas::vector<double>& s1, ublas::vector<double>& s2) const
{
    assert(s1.size() == s2.size());

    s2 = Random::Instance().GaussianMultiVariate(s1, fCholeskyDecomp);

    return 1.0;
}

} /* namespace fmcmc */
