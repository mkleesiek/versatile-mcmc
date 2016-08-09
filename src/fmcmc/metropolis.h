/*
 * metropolis.h
 *
 *  Created on: 29.07.2016
 *      Author: marco@kleesiek.com
 */

#ifndef SRC_FMCMC_METROPOLIS_H_
#define SRC_FMCMC_METROPOLIS_H_

#include <fmcmc/algorithm.h>

#include <memory>

namespace fmcmc
{

class Proposal;

class MetropolisHastings: public Algorithm
{
public:
    MetropolisHastings();
    virtual ~MetropolisHastings();

    virtual bool Initialize() override;

    virtual double Advance() override;

    virtual size_t NChains() override { return fSampledChains.size(); }
    virtual const Chain& GetChain(size_t cIndex = 0) override { return fSampledChains[cIndex]; }

    double GetBeta(size_t cIndex = 0) { return fBetas[cIndex]; }

protected:
    std::unique_ptr<Proposal> fProposalFunction;
    double fInitialErrorScale;
    double fStartPointRandomization;

    std::vector<double> fBetas;
    std::vector<Chain> fSampledChains;
};

} /* namespace fmcmc */

#endif /* SRC_FMCMC_METROPOLIS_H_ */
