/**
 * @file
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 */

#ifndef VMCMC_PROPOSAL_H_
#define VMCMC_PROPOSAL_H_

#include <vmcmc/blas.hpp>
#include <vmcmc/parameter.hpp>
#include <vmcmc/sample.hpp>
#include <vmcmc/random.hpp>

namespace vmcmc
{

/**
 * Base class for proposal functions (alias transition kernels) used by
 * Metropolis-Hastings algorithms to propose a new point in the parameter
 * space.
 */
class Proposal
{
public:
    Proposal() { }
    virtual ~Proposal() { }

    virtual Proposal* Clone() const = 0;

    /**
     * Propose a new state.
     * @param[in] s1 The original state to start from.
     * @param[out] s2 The new proposed state.
     * @return The asymmetry ratio. (1.0 for symmetric proposal functions).
     */
    virtual double Transition(const Vector& s1, Vector& s2) = 0;

    double Transition(const Sample& s1, Sample& s2) { return Transition(s1.Values(), s2.Values()); }

    virtual void UpdateParameterConfig(const ParameterConfig& /*paramConfig*/) { };
};


/**
 * Abstract proposal function class, which randomly draws from a multivariate random distribution
 * centered at the previous parameter vector. If the full covariance matrix of
 * the normal distribution is provided, correlations are taken into account.
 */
template<class DistributionT>
class ProposalDistribution : public Proposal
{
public:
    ProposalDistribution() :
        fCholeskyDecomp( ublas::identity_matrix<double>() )
    { }
    virtual ~ProposalDistribution() { }

    double Transition(const Vector& s1, Vector& s2) override;
    using Proposal::Transition;

    void UpdateParameterConfig(const ParameterConfig& paramConfig) override;

protected:
    DistributionT fDistribution;
    MatrixLower fCholeskyDecomp;
};


class ProposalNormal : public ProposalDistribution< std::normal_distribution<double> >
{
public:
    ProposalNormal() { }
    virtual ~ProposalNormal() { }

    virtual ProposalNormal* Clone() const override { return new ProposalNormal(*this); }
};

class ProposalStudentT : public ProposalDistribution< std::student_t_distribution<double> >
{
public:
    ProposalStudentT(double dof = 1.0) { SetDOF(dof); }
    virtual ~ProposalStudentT() { }

    virtual ProposalStudentT* Clone() const override { return new ProposalStudentT(*this); }

    void SetDOF(double n = 1.0) { fDistribution = std::student_t_distribution<double>(n); }
    double GetDOF() const { return fDistribution.n(); }
};

} /* namespace vmcmc */

#endif /* VMCMC_PROPOSAL_H_ */
