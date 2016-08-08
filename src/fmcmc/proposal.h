/*
 * Proposal.h
 *
 *  Created on: 31.07.2016
 *      Author: marco@kleesiek.com
 */

#ifndef SRC_FMCMC_PROPOSAL_H_
#define SRC_FMCMC_PROPOSAL_H_

#include <fmcmc/ublas.h>

namespace fmcmc
{

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
};

class ProposalGaussian : public Proposal
{
public:
    ProposalGaussian();
    ~ProposalGaussian() override;

    double Transition(const ublas::vector<double>& s1, ublas::vector<double>& s2) const override;

protected:
    ublas::triangular_matrix<double, ublas::lower> fCholeskyDecomp;
};

} /* namespace fmcmc */

#endif /* SRC_FMCMC_PROPOSAL_H_ */
