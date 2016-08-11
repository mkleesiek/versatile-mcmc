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
#include <algorithm>
#include <initializer_list>

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

    template<class ContainerT = std::initializer_list<double>>
    void SetBetas(ContainerT betas);
    const std::vector<double>& GetBetas() const { return fBetas; }

protected:
    double fInitialErrorScale;
    double fStartPointRandomization;

    std::vector<double> fBetas;
    std::vector<std::unique_ptr<Proposal>> fProposalFunctions;
    std::vector<Chain> fSampledChains;
};

template<class ContainerT>
inline void MetropolisHastings::SetBetas(ContainerT betas)
{
    fBetas = { 1.0 };
    std::copy_if( betas.begin(), betas.end(),
        std::back_inserter(fBetas), [](double beta){ return beta > 1.0; } );
    std::sort(fBetas.begin(), fBetas.end());
}

} /* namespace fmcmc */

#endif /* SRC_FMCMC_METROPOLIS_H_ */
