/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <logger.hpp>
#include <metropolis.hpp>
#include <proposal.hpp>
#include <random.hpp>
#include <stringutils.hpp>
#include <algorithm>

#ifdef USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
using namespace tbb;
#endif // USE_TBB

using namespace std;

namespace vmcmc
{

LOG_DEFINE("vmcmc.metropolis");

/**
 * Private class encapsulating Sample chains for parallel tempering.
 *
 * A ChainConfig describes a set of parallel tempered chains, each chain
 * with its own copy of an adjusted parameter configuration and proposal
 * function.
 * If parallel tempering is not used (only one beta value = 1.0 defined),
 * a ChainConfig contains only one "cold" chain.
 */
struct MetropolisHastings::ChainConfig
{
    ChainConfig(size_t n, const ParameterConfig& initialParamConf,
            const Proposal* propFunc = nullptr) :
        // in case of parallel tempering, setup more than one chain
        fPtChains( n, Chain() ),
        // prepare parameter configurations
        fDynamicParamConfigs( n, initialParamConf ),
        // clone the default proposal function
        fProposalFunctions( n )
    {
        LOG_ASSERT( n > 0, "A Metropolis chain set requires at least 1 chain"
             << " (and corresponding beta value).");

        if (propFunc)
            for (auto& p : fProposalFunctions)
                p.reset( propFunc->Clone() );

        if (n > 0) {
            fNProposedSwaps.assign( n-1, 0 );
            fNAcceptedSwaps.assign( n-1, 0 );
        }
    }

    std::vector<Chain> fPtChains;
    std::vector<ParameterConfig> fDynamicParamConfigs;
    std::vector<std::unique_ptr<Proposal>> fProposalFunctions;
    std::vector<size_t> fNProposedSwaps;
    std::vector<size_t> fNAcceptedSwaps;
};

MetropolisHastings::MetropolisHastings() :
    fRandomizeStartPoint( false ),
    fBetas{ 1.0 },
    fPtFrequency( 200 ),
    fChainConfigs( 1 ),
    fMultiThreading( true )
{
#ifndef USE_TBB
    fMultiThreading = false;
#endif
}

MetropolisHastings::~MetropolisHastings()
{ }

void MetropolisHastings::SetMultiThreading(bool enable)
{
#ifndef USE_TBB
    if (enable)
        LOG(Warn, "TBB is not available for multi-threading.");
    fMultiThreading = false;
#endif
    fMultiThreading = enable;
}

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

double MetropolisHastings::GetSwapAcceptanceRate(size_t iChain, ptrdiff_t iBeta) const
{
    if (iChain >= fChainConfigs.size())
        return 0.0;

    const auto& chainConfig = fChainConfigs[iChain];

    if (iBeta < 0) {
        const double nAcceptedSwaps = accumulate(
            chainConfig->fNAcceptedSwaps.begin(),
            chainConfig->fNAcceptedSwaps.end(), 0 );

        const double nProposedSwaps = accumulate(
            chainConfig->fNProposedSwaps.begin(),
            chainConfig->fNProposedSwaps.end(), 0 );

        return nAcceptedSwaps / nProposedSwaps;
    }
    else if ((size_t) iBeta >= chainConfig->fNAcceptedSwaps.size()) {
        return 0.0;
    }
    else {
        return chainConfig->fNAcceptedSwaps[iBeta] / chainConfig->fNProposedSwaps[iBeta];
    }


}

void MetropolisHastings::Initialize()
{
    Algorithm::Initialize();

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

        chainConfig.reset(
            new ChainConfig(nBetas, fParameterConfig, fProposalFunction.get()) );

        // for each PT (beta) chain, setup an individual parameter configuration
        const double initialErrorScaling = fParameterConfig.GetErrorScaling();
        for (size_t iBeta = 0; iBeta < nBetas; iBeta++) {

            // scale parameter configurations
            if (iBeta > 0)
                chainConfig->fDynamicParamConfigs[iBeta].SetErrorScaling(
                    initialErrorScaling / sqrt(fBetas[iBeta]) );

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

            chain.reserve( GetTotalLength()+1 );

            chain.push_back( startPoint );
        }
    }
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

    /**
     * In case of multi-core parallelization, all active chains
     * (number of chain sets * number of PT beta values)
     * are progressed in parallel by nSteps each.
     */
    if (fMultiThreading) {
#ifdef USE_TBB
        const size_t nTotalChains = nChainConfigs * nBetas;

        parallel_for(
            blocked_range<size_t>(0, nTotalChains),
            [&](const blocked_range<size_t>& range) {
                for (size_t iChain = range.begin(); iChain < range.end(); iChain++) {
                    const size_t iChainConfig = iChain / nBetas;
                    const size_t iBeta = iChain % nBetas;
                    this->AdvanceChainConfig( iChainConfig, iBeta, nSteps );
                }
            }
        );
#else
        LOG(Fatal, "TBB not available - multi-threading should be deactivated.");
#endif
    }
    else {
        for (size_t iChainConfig = 0; iChainConfig < nChainConfigs; iChainConfig++)
            for (size_t iBeta = 0; iBeta < nBetas; iBeta++)
                AdvanceChainConfig( iChainConfig, iBeta, nSteps );
    }

    if (nBetas < 2)
        return;

    // propose sample swaps between tempered chains in each chain config:
    const double swapProposalProb = (double) nSteps / (double) fPtFrequency;

    for (size_t iChainConfig = 0; iChainConfig < nChainConfigs; iChainConfig++)
        if ( Random::Instance().Bool( swapProposalProb ) )
            ProposePtSwapping(iChainConfig);
}

void MetropolisHastings::Finalize()
{
    const size_t nBetas = fBetas.size();

    if (nBetas < 2) {
        LOG(Info, "No parallel tempering.");
    }
    else {
        const size_t nChainConfigs = fChainConfigs.size();

        for (size_t i = 0 ; i < nChainConfigs; i++) {

            // output the individual acceptance rates
            vector<double> accRates(nBetas, 0.0);
            for (size_t b = 0 ; b < nBetas; b++) {
                ChainStatistics stats( fChainConfigs[i]->fPtChains[b] );
                accRates[b] = stats.GetAccRate();
            }
            LOG(Info, "Metrop. acc. rates in chain set " << i << ": " << accRates);

            // output the parallel tempering swap acceptance rates
            vector<double> swapRates(nBetas-1, 0.0);
            for (size_t b = 0 ; b < nBetas-1; b++) {
                swapRates[b] = (double) fChainConfigs[i]->fNAcceptedSwaps[b]
                     / fChainConfigs[i]->fNProposedSwaps[b];
            }
            LOG(Info, "PT swap acc. rates in chain set " << i << ": " << swapRates);
        }
    }

    Algorithm::Finalize();
}

void MetropolisHastings::AdvanceChainConfig(size_t iChainConfig, size_t iBeta, size_t nSteps)
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

void MetropolisHastings::ProposePtSwapping(size_t iChainConfig)
{
    if (fBetas.size() < 2)
        return;

    auto& chainConfig = fChainConfigs[iChainConfig];

    // randomly pick 2 adjacent chains
    const size_t colderChainIndex = Random::Instance().Uniform<size_t>(0, fBetas.size()-2);

    Chain& colderChain = chainConfig->fPtChains[colderChainIndex];
    const double colderBeta = fBetas[colderChainIndex];

    Chain& warmerChain = chainConfig->fPtChains[colderChainIndex+1];
    const double warmerBeta = fBetas[colderChainIndex+1];

    const double colderNegLogL = colderChain.back().GetNegLogLikelihood();
    const double warmerNegLogL = warmerChain.back().GetNegLogLikelihood();

    // calculate the swap probability
    const double ptRatio = std::min(1.0, exp(
            colderBeta * (colderNegLogL-warmerNegLogL)
          + warmerBeta * (warmerNegLogL-colderNegLogL)
    ) );

    chainConfig->fNProposedSwaps[colderChainIndex]++;

    const bool performSwap = Random::Instance().Bool( ptRatio );
    if (performSwap) {
        LOG(Debug, "Sampler " << colderChainIndex << " and " << colderChainIndex+1 << " swapped.");
        swap(colderChain.back(), warmerChain.back());

        chainConfig->fNAcceptedSwaps[colderChainIndex]++;
    }
}

} /* namespace vmcmc */
