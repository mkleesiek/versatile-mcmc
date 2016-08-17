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
    static double CalculateMHRatio(const Sample& prevState, const Sample& nextState,
        double proposalAsymmetry = 1.0, double beta = 1.0);

public:
    MetropolisHastings();
    virtual ~MetropolisHastings();

    virtual bool Initialize() override;

    virtual void Advance() override;

    virtual size_t NChains() override { return fSampledChains.size(); }
    virtual const Chain& GetChain(size_t cIndex = 0) override { return fSampledChains[cIndex]; }

    template<class ContainerT = std::initializer_list<double>>
    void SetBetas(ContainerT betas);
    const std::vector<double>& GetBetas() const { return fBetas; }

    void SetProposalFunction(std::shared_ptr<Proposal> proposalFunction) { fProposalFunctions[0] = proposalFunction; }
    std::shared_ptr<Proposal> GetProposalFunction() { return fProposalFunctions[0]; };
    std::shared_ptr<const Proposal> GetProposalFunction() const { return fProposalFunctions[0]; }

    void SetRandomizeStartPoint(bool randomizeStartPoint) { fRandomizeStartPoint = randomizeStartPoint; }
    bool IsRandomizeStartPoint() const { return fRandomizeStartPoint; }

protected:
    void AdvanceChain(size_t iChain);

    bool fRandomizeStartPoint;

    std::vector<std::shared_ptr<Proposal>> fProposalFunctions;
    std::vector<double> fBetas;
    std::vector<ParameterList> fDynamicParamConfigs;
    std::vector<Chain> fSampledChains;
};

template<class ContainerT>
inline void MetropolisHastings::SetBetas(ContainerT betas)
{
    // make sure, that the default chain (0) has nominal temperature
    fBetas = { 1.0 };

    // only add betas < 1.0 (higher temperature)
    std::copy_if( betas.begin(), betas.end(),
        std::back_inserter(fBetas), [](double beta){ return beta < 1.0; } );

    // and sort in descending order
    std::sort(fBetas.begin(), fBetas.end(), std::greater<double>());
}

} /* namespace vmcmc */

#endif /* VMCMC_METROPOLIS_H_ */
