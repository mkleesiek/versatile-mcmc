/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 *
 * @brief Proposal functions (defined as classes) for the Metropolis-Hastings
 * algorithm.
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
 * Abstract proposal function class, which randomly draws from a multivariate
 * random distribution centered at current sample / parameter vector.
 * If the full covariance matrix of the parameter configuration is provided,
 * correlations are taken into account when setting up the random distribution.
 */
template <typename DistributionT>
class ProposalDistribution : public Proposal
{
public:
    ProposalDistribution(DistributionT&& dist = DistributionT()) :
        fDistribution( std::forward<DistributionT>(dist) ),
        fCholeskyDecomp( ublas::identity_matrix<double>() )
    { }
    virtual ~ProposalDistribution() { }

    double Transition(const Vector& s1, Vector& s2) override;
    using Proposal::Transition;

    void UpdateParameterConfig(const ParameterConfig& paramConfig) override;

    const MatrixLower& GetCholeskyDecomp() const { return fCholeskyDecomp; }

protected:
    DistributionT fDistribution;
    MatrixLower fCholeskyDecomp;
};

/**
 * Proposal function drawing randomly from a multivariate normal distribution.
 */
class ProposalNormal : public ProposalDistribution< std::normal_distribution<double> >
{
public:
    ProposalNormal() { }
    virtual ~ProposalNormal() { }

    virtual ProposalNormal* Clone() const override { return new ProposalNormal(*this); }
};

/**
 * Proposal function drawing randomly from a multivariate Student-T distribution.
 */
class ProposalStudentT : public ProposalDistribution< std::student_t_distribution<double> >
{
public:
    ProposalStudentT(double dof = 1.0) :
        ProposalDistribution(std::student_t_distribution<double>(dof)) { }
    virtual ~ProposalStudentT() { }

    virtual ProposalStudentT* Clone() const override { return new ProposalStudentT(*this); }

    void SetDOF(double n = 1.0) { fDistribution = std::student_t_distribution<double>(n); }
    double GetDOF() const { return fDistribution.n(); }
};

} /* namespace vmcmc */

#endif /* VMCMC_PROPOSAL_H_ */
