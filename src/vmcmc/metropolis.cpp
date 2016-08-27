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

MetropolisHastings::MetropolisHastings() :
    fRandomizeStartPoint( false ),
    fBetas{ 1.0 },
    fProposalFunctions( {nullptr} ),
    fPtFrequency( 100 )
{ }

MetropolisHastings::~MetropolisHastings()
{ }

bool MetropolisHastings::Initialize()
{
    if (!Algorithm::Initialize())
        return false;

    if (fBetas.empty())
        fBetas = { 1.0 };

    const size_t nChains = fBetas.size();

    // in case of parallel tempering, setup more than one chain
    fSampledChains.assign( nChains, Chain() );

    // for each chain, setup an individual parameter configuration
    const double initialErrorScaling = fParameterConfig.GetErrorScaling();
    fDynamicParamConfigs.assign( nChains, fParameterConfig );
    for (size_t iChain = 1; iChain < nChains; ++iChain) {
        fDynamicParamConfigs[iChain].SetErrorScaling( initialErrorScaling / sqrt(fBetas[iChain]) );
    }

    // if require, randomize the starting vector for each chain
    if (fRandomizeStartPoint) {
        for (auto& chain : fSampledChains) {
            Sample startPoint( GetParameterConfig().GetStartValues(true) );
            Evaluate( startPoint );
            startPoint.SetAccepted( true );
            chain.push_back( startPoint );
        }
    }
    else {
        Sample startPoint( GetParameterConfig().GetStartValues(false) );
        Evaluate( startPoint );
        startPoint.SetAccepted( true );
        for (auto& chain : fSampledChains)
            chain.push_back( startPoint );
    }


    fProposalFunctions.resize(nChains);
    // if not set by the user, instantiate default proposal function
    if (!fProposalFunctions[0]) {
        LOG(Info, "Using default proposal function 'ProposalGaussian'.");
        fProposalFunctions = { make_shared<ProposalGaussian>() };
    }
    for (size_t iChain = 0; iChain < nChains; ++iChain) {
        // clone the proposal function from chain 0 to all others
        if (iChain > 0)
            fProposalFunctions[iChain].reset( fProposalFunctions[0]->Clone() );
        // update parameter configuration
        fProposalFunctions[iChain]->UpdateParameterConfig( fDynamicParamConfigs[iChain] );
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
    const size_t nChains = fSampledChains.size();

#ifdef USE_TBB
    parallel_for(
        blocked_range<size_t>(0,nChains),
        [&](const blocked_range<size_t>& range) {
            for (size_t iChain = range.begin(); iChain < range.end(); iChain++)
                this->AdvanceChain( iChain, nSteps );
        }
    );
#else
    for (size_t iChain = 0; iChain < nChains; iChain++)
        AdvanceChain( iChain, nSteps );
#endif

    // propose sample swaps between chains:
    const bool doProposeSwap = nChains > 1 && Random::Instance().Bool( (double) nSteps / (double) fPtFrequency );
    if (doProposeSwap) {
        const size_t colderChainIndex = Random::Instance().Uniform<size_t>(0, nChains-2);

        Chain& colderChain = fSampledChains[colderChainIndex];
        const double colderBeta = fBetas[colderChainIndex];

        Chain& warmerChain = fSampledChains[colderChainIndex+1];
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

void MetropolisHastings::AdvanceChain(size_t iChain, size_t nSteps)
{
    LOG_ASSERT( fProposalFunctions[iChain], "No proposal function defined." );

    auto& chain = fSampledChains[iChain];
    LOG_ASSERT( !chain.empty(), "No starting point in chain " << iChain << "." );

    for (size_t iStep = 0; iStep < nSteps; iStep++) {

        const Sample& previousState = chain.back();

        // prepare the upcoming sample
        Sample nextState( previousState );
        nextState.IncrementGeneration();
        nextState.Reset();

        // propose the next point in the parameter space
        const double proposalAsymmetry = fProposalFunctions[iChain]->Transition( previousState, nextState );

        // attempt reflection if limits are exceeded
        fDynamicParamConfigs[iChain].ReflectFromLimits( nextState.Values() );

        // evaluate likelihood and prior
        Evaluate( nextState );

        const double mhRatio = CalculateMHRatio(previousState, nextState, proposalAsymmetry, fBetas[iChain]);

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
