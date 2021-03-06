/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 */

#include <logger.hpp>
#include <proposal.hpp>
#include <random.hpp>

namespace vmcmc
{

LOG_DEFINE("vmcmc.proposal");

template <typename DistributionT>
double ProposalDistribution<DistributionT>::Transition(const Vector& s1, Vector& s2)
{
    LOG_ASSERT(s1.size() == s2.size());
    LOG_ASSERT(s1.size() == fCholeskyDecomp.size1());

    s2 = Random::Instance().FromMultiVariateDistribution(
        fDistribution, s1, fCholeskyDecomp );

    return 1.0;
}

template <typename DistributionT>
void ProposalDistribution<DistributionT>::UpdateParameterConfig(const ParameterConfig& paramConfig)
{
    Proposal::UpdateParameterConfig(paramConfig);
    fCholeskyDecomp = paramConfig.GetCholeskyDecomp();
}

// explicit instantiation definitions
template class ProposalDistribution< std::normal_distribution<double> >;
template class ProposalDistribution< std::student_t_distribution<double> >;

} /* namespace vmcmc */
