/**
 * @file
 *
 * @date 31.07.2016
 * @author marco@kleesiek.com
 */

#ifndef SRC_FMCMC_PROPOSAL_H_
#define SRC_FMCMC_PROPOSAL_H_

#include <fmcmc/ublas.h>
#include <fmcmc/parameter.h>

namespace fmcmc
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
     * @param s1 The original state to start from.
     * @param s2 The new proposed state.
     * @return The asymmetry ratio. (1.0 for symmetric proposal functions).
     */
    virtual double Transition(const ublas::vector<double>& s1, ublas::vector<double>& s2) const = 0;

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

    double Transition(const ublas::vector<double>& s1, ublas::vector<double>& s2) const override;

    void SetParameterConfig(const ParameterSet& paramConfig) override;

//    void SetStandardDeviations(const ublas::vector<double>& errors);
//    void SetCovarianceMatrix(const ublas::triangular_matrix<double, ublas::lower>& cov);

protected:
    ublas::triangular_matrix<double, ublas::lower> fCholeskyDecomp;
};

} /* namespace fmcmc */

#endif /* SRC_FMCMC_PROPOSAL_H_ */
