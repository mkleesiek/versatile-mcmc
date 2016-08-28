/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/metropolis.h>
#include <vmcmc/proposal.h>
#include <vmcmc/random.h>
#include <vmcmc/logger.h>

#include <algorithm>

#ifdef USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
using namespace tbb;
#endif

using namespace std;

namespace vmcmc
{

LOG_DEFINE("vmcmc.metropolis");

struct MetropolisHastings::ChainConfig
{
    std::vector<Chain> fPtChains;
    std::vector<ParameterConfig> fDynamicParamConfigs;
    std::vector<std::unique_ptr<Proposal>> fProposalFunctions;
};

MetropolisHastings::MetropolisHastings() :
    fRandomizeStartPoint( false ),
    fBetas{ 1.0 },
    fPtFrequency( 100 ),
    fChainConfigs( 1 )
{ }

MetropolisHastings::~MetropolisHastings()
{ }

const Chain& MetropolisHastings::GetChain(size_t cIndex)
{
    assert( fChainConfigs.size() > cIndex && fChainConfigs[cIndex]
            && !fChainConfigs[cIndex]->fPtChains.empty() );

    // return the 'cold' sampled chain
    return fChainConfigs[cIndex]->fPtChains.front();
}

void MetropolisHastings::SetNumberOfChains(size_t nChains)
{
    fChainConfigs.resize( std::max<size_t>(nChains, 1) );
}

bool MetropolisHastings::Initialize()
{
    if (!Algorithm::Initialize())
        return false;

    if (fBetas.empty())
        fBetas = { 1.0 };

//    const size_t nChainConfigs = fChains.size();
    const size_t nBetas = fBetas.size();

    // global start point in parameter space
    Sample startPoint( fParameterConfig.GetStartValues(false) );
    Evaluate( startPoint );
    startPoint.SetAccepted( true );

    // if not set by the user, instantiate default proposal function
    if (!fProposalFunction) {
        LOG(Info, "Using default proposal function 'ProposalGaussian'.");
        fProposalFunction = make_shared<ProposalNormal>();
    }

    // initialize chain configurations
    for (auto& chainConfig : fChainConfigs) {
        chainConfig.reset( new ChainConfig() );

        // in case of parallel tempering, setup more than one chain
        chainConfig->fPtChains.assign( nBetas, Chain() );
        // clone the default proposal function
        chainConfig->fProposalFunctions.resize( nBetas );
        // prepare parameter configurations
        chainConfig->fDynamicParamConfigs.assign( nBetas, fParameterConfig );

        // for each PT (beta) chain, setup an individual parameter configuration
        const double initialErrorScaling = fParameterConfig.GetErrorScaling();
        for (size_t iBeta = 0; iBeta < nBetas; iBeta++) {

            // scale parameter configurations
            if (iBeta > 0)
                chainConfig->fDynamicParamConfigs[iBeta].SetErrorScaling( initialErrorScaling / sqrt(fBetas[iBeta]) );

            chainConfig->fProposalFunctions[iBeta].reset( fProposalFunction->Clone() );

            // update proposal functions with parameter configuration
            chainConfig->fProposalFunctions[iBeta]->UpdateParameterConfig(
                chainConfig->fDynamicParamConfigs[iBeta] );
        }

        // setup start points
        for (auto& chain : chainConfig->fPtChains) {
            // if required, randomize the starting vector for each chain
            if (fRandomizeStartPoint) {
                startPoint.Values() = GetParameterConfig().GetStartValues(true);
                Evaluate( startPoint );
            }

            chain.push_back( startPoint );
        }
    }

    return true;
}

double MetropolisHastings::CalculateMHRatio(const Sample& prevState, const Sample& nextState,
    double proposalAsymmetry, double beta)
{
    if (nextState.GetPrior() == 0.0)
        return 0.0;

    // calculate Metropolis-Hastings ratio
    return std::min(1.0, proposalAsymmetry
        * nextState.GetPrior()/prevState.GetPrior()
        * exp( beta * (prevState.GetNegLogLikelihood() - nextState.GetNegLogLikelihood()) )
    );
}

void MetropolisHastings::Advance(size_t nSteps)
{
    const size_t nChainConfigs = fChainConfigs.size();
    const size_t nBetas = fBetas.size();

#ifdef USE_TBB
    const size_t nTotalChains = nChainConfigs * nBetas;

    parallel_for(
        blocked_range<size_t>(0, nTotalChains),
        [&](const blocked_range<size_t>& range) {
            for (size_t iChain = range.begin(); iChain < range.end(); iChain++) {
                const size_t iChainConfig = iChain / nBetas;
                const size_t iBeta = iChain % nBetas;
                this->AdvanceChain( iChainConfig, iBeta, nSteps );
            }
        }
    );
#else
    for (size_t iChainConfig = 0; iChainConfig < nChainConfigs; iChainConfig++)
        for (size_t iBeta = 0; iBeta < nBetas; iBeta++)
            AdvanceChain( iChainConfig, iBeta, nSteps );
#endif

    if (nBetas <= 1)
        return;

    // propose sample swaps between tempered chains in each chain config:

    for (auto& chainConfig : fChainConfigs) {

        const bool doProposeSwap = Random::Instance().Bool( (double) nSteps / (double) fPtFrequency );
        if (doProposeSwap) {
            const size_t colderChainIndex = Random::Instance().Uniform<size_t>(0, nBetas-2);

            Chain& colderChain = chainConfig->fPtChains[colderChainIndex];
            const double colderBeta = fBetas[colderChainIndex];

            Chain& warmerChain = chainConfig->fPtChains[colderChainIndex+1];
            const double warmerBeta = fBetas[colderChainIndex+1];

            const double colderNegLogL = colderChain.back().GetNegLogLikelihood();
            const double warmerNegLogL = warmerChain.back().GetNegLogLikelihood();

            const double ptRatio = std::min(1.0, exp(
                    colderBeta * (colderNegLogL-warmerNegLogL)
                  + warmerBeta * (warmerNegLogL-colderNegLogL)
            ) );

            const bool performSwap = Random::Instance().Bool( ptRatio );
            if (performSwap) {
                LOG(Debug, "Sampler " << colderChainIndex << " and " << colderChainIndex+1 << " swapped.");
                swap(colderChain.back(), warmerChain.back());
            }
        }
    }
}

void MetropolisHastings::AdvanceChain(size_t iChainConfig, size_t iBeta, size_t nSteps)
{
    LOG_ASSERT( fChainConfigs.size() > iChainConfig && fChainConfigs[iChainConfig],
        "Chain configuration " << iChainConfig << " not initialized.");

    ChainConfig& chainConfig = *fChainConfigs[iChainConfig];

    LOG_ASSERT( chainConfig.fPtChains.size() > iBeta,
        "Chain for beta index " << iBeta << " not initialized.");

    LOG_ASSERT( chainConfig.fProposalFunctions[iBeta], "No proposal function defined." );

    Proposal* proposal = chainConfig.fProposalFunctions[iBeta].get();
    ParameterConfig& paramConfig = chainConfig.fDynamicParamConfigs[iBeta];

    Chain& chain = chainConfig.fPtChains[iBeta];

    LOG_ASSERT( !chain.empty(), "No starting point in chain " << iChainConfig
        << "/" << iBeta << "." );

    for (size_t iStep = 0; iStep < nSteps; iStep++) {

        const Sample& previousState = chain.back();

        // prepare the upcoming sample
        Sample nextState( previousState );
        nextState.IncrementGeneration();
        nextState.Reset();

        // propose the next point in the parameter space
        const double proposalAsymmetry = proposal->Transition( previousState, nextState );

        // attempt reflection if limits are exceeded
        paramConfig.ReflectFromLimits( nextState.Values() );

        // evaluate likelihood and prior
        Evaluate( nextState );

        const double mhRatio = CalculateMHRatio( previousState, nextState,
            proposalAsymmetry, fBetas[iBeta] );

        const bool proposalAccepted = Random::Instance().Bool( mhRatio );

        if (proposalAccepted) {
            nextState.SetAccepted( true );
            chain.push_back( nextState );
        }
        else {
            nextState = previousState;
            nextState.SetAccepted( false );
            nextState.IncrementGeneration();
            chain.push_back( nextState );
        }
    }
}

} /* namespace vmcmc */
