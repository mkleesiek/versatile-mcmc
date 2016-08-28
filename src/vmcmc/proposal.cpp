/**
 * @file
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/proposal.h>
#include <vmcmc/random.h>
#include <vmcmc/logger.h>

namespace vmcmc
{

LOG_DEFINE("vmcmc.proposal");

template<class DistributionT>
double ProposalDistribution<DistributionT>::Transition(const Vector& s1, Vector& s2)
{
    LOG_ASSERT(s1.size() == s2.size());
    LOG_ASSERT(s1.size() == fCholeskyDecomp.size1());

    s2 = Random::Instance().FromMultiVariateDistribution(
        fDistribution, s1, fCholeskyDecomp );

    return 1.0;
}

template<class DistributionT>
void ProposalDistribution<DistributionT>::UpdateParameterConfig(const ParameterConfig& paramConfig)
{
    fCholeskyDecomp = paramConfig.GetCholeskyDecomp();
}

// template specializations
template class ProposalDistribution< std::normal_distribution<double> >;
template class ProposalDistribution< std::student_t_distribution<double> >;

} /* namespace vmcmc */
