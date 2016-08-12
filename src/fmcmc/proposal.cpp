/**
 * @file
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 */

#include <fmcmc/proposal.h>
#include <fmcmc/random.h>
#include <fmcmc/logger.h>
#include <fmcmc/stringutils.h>

#include <cmath>

namespace fmcmc
{

LOG_DEFINE("fmcmc.proposal");

Proposal::Proposal()
{ }

Proposal::~Proposal()
{ }

ProposalGaussian::ProposalGaussian()
//    fCholeskyDecomp( ublas::identity_matrix<double>() )
{ }

ProposalGaussian::~ProposalGaussian()
{ }

//void ProposalGaussian::SetStandardDeviations(const ublas::vector<double>& errors)
//{
//    const size_t n = errors.size();
//
//    if (fCholeskyDecomp.size1() != n)
//        fCholeskyDecomp.resize(n, n, false);
//
//    for (size_t i = 0; i < n; i++) {
//        for (size_t j = 0; j < i; j++)
//            fCholeskyDecomp(i, j) = 0.0;
//        fCholeskyDecomp(i, i) = errors[i];
//    }
//}
//
//void ProposalGaussian::SetCovarianceMatrix(const ublas::triangular_matrix<double, ublas::lower>& cov)
//{
//    const size_t n = cov.size1();
//
//    if (fCholeskyDecomp.size1() != n)
//        fCholeskyDecomp.resize(n, n, false);
//
//    const size_t statusDecomp = choleskyDecompose(cov, fCholeskyDecomp);
//    if (statusDecomp != 0) {
//        LOG(Error, "Cholesky decomposition of covariance matrix " << cov << " failed.");
//
//        for (size_t i = 0; i < n; i++) {
//            for (size_t j = 0; j < i; j++)
//                fCholeskyDecomp(i, j) = 0.0;
//            fCholeskyDecomp(i, i) = sqrt( cov(i, i) );
//        }
//    }
//}

double ProposalGaussian::Transition(const ublas::vector<double>& s1, ublas::vector<double>& s2) const
{
    assert(s1.size() == s2.size());
    assert(s1.size() == fCholeskyDecomp.size1());

    s2 = Random::Instance().GaussianMultiVariate(s1, fCholeskyDecomp);

    return 1.0;
}

void ProposalGaussian::SetParameterConfig(const ParameterSet& paramConfig)
{
    fCholeskyDecomp = paramConfig.GetCholeskyDecomp();
}

} /* namespace fmcmc */
