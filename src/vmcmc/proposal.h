/**
 * @file
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 */

#ifndef VMCMC_PROPOSAL_H_
#define VMCMC_PROPOSAL_H_

#include <vmcmc/sample.h>
#include <vmcmc/parameter.h>
#include <vmcmc/blas.h>

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
    Proposal();
    virtual ~Proposal();

    /**
     * Propose a new state.
     * @param[in] s1 The original state to start from.
     * @param[out] s2 The new proposed state.
     * @return The asymmetry ratio. (1.0 for symmetric proposal functions).
     */
    virtual double Transition(const Vector& s1, Vector& s2) const = 0;

    double Transition(const Sample& s1, Sample& s2);

    virtual void SetParameterConfig(const ParameterSet& /*paramConfig*/) { };
};

/**
 * Proposal function, which randomly draws from a multivariate normal distribution
 * centered at the previous parameter vector. If the full covariance matrix of
 * the normal distribution is provided, correlations are taken into account.
 */
class ProposalGaussian : public Proposal
{
public:
    ProposalGaussian();
    ~ProposalGaussian();

    double Transition(const Vector& s1, Vector& s2) const override;

    void SetParameterConfig(const ParameterSet& paramConfig) override;

protected:
    MatrixLower fCholeskyDecomp;
};

inline double Proposal::Transition(const Sample& s1, Sample& s2)
{
    return Transition(s1.Values(), s2.Values());
}

} /* namespace vmcmc */

#endif /* VMCMC_PROPOSAL_H_ */
