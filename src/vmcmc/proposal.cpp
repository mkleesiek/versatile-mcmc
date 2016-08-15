/**
 * @file
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/proposal.h>
#include <vmcmc/random.h>
#include <vmcmc/logger.h>
#include <vmcmc/stringutils.h>

#include <cmath>

namespace vmcmc
{

LOG_DEFINE("vmcmc.proposal");

Proposal::Proposal()
{ }

Proposal::~Proposal()
{ }

ProposalGaussian::ProposalGaussian() :
    fCholeskyDecomp( ublas::identity_matrix<double>() )
{ }

ProposalGaussian::~ProposalGaussian()
{ }

double ProposalGaussian::Transition(const Vector& s1, Vector& s2) const
{
    LOG_ASSERT(s1.size() == s2.size());
    LOG_ASSERT(s1.size() == fCholeskyDecomp.size1());

    s2 = Random::Instance().GaussianMultiVariate(s1, fCholeskyDecomp);

    return 1.0;
}

void ProposalGaussian::SetParameterConfig(const ParameterList& paramConfig)
{
    fCholeskyDecomp = paramConfig.GetCholeskyDecomp();
}

} /* namespace vmcmc */
