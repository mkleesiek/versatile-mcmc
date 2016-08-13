/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#ifndef VMCMC_METROPOLIS_H_
#define VMCMC_METROPOLIS_H_

#include <vmcmc/algorithm.h>

#include <memory>
#include <algorithm>
#include <initializer_list>

namespace vmcmc
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

    void SetProposalFunction(std::shared_ptr<Proposal> proposalFunction) { fProposalFunction = proposalFunction; }
    std::shared_ptr<Proposal> GetProposalFunction() { return fProposalFunction; };
    std::shared_ptr<const Proposal> GetProposalFunction() const { return fProposalFunction; };

protected:
    double fInitialErrorScale;
    double fStartPointRandomization;
    std::shared_ptr<Proposal> fProposalFunction;

    std::vector<double> fBetas;
    std::vector<ParameterSet> fDynamicParamConfigs;
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

} /* namespace vmcmc */

#endif /* VMCMC_METROPOLIS_H_ */
