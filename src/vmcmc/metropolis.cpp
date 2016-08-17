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
    fProposalFunctions( {nullptr} ),
    fBetas{ 1.0 }
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
        fDynamicParamConfigs[iChain].SetErrorScaling( initialErrorScaling * exp(1.0 - fBetas[iChain]) );
    }

    // if require, randomize the starting vector for each chain
    if (fRandomizeStartPoint) {
        for (auto& chain : fSampledChains) {
            Sample startPoint( GetParameterConfig().GetStartValues(true) );
            Evaluate( startPoint );
            chain.push_back( startPoint );
        }
    }
    else {
        Sample startPoint( GetParameterConfig().GetStartValues(false) );
        Evaluate( startPoint );
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

void MetropolisHastings::Advance()
{
    const size_t nChains = fSampledChains.size();

#ifdef USE_TBB
    parallel_for(
        blocked_range<size_t>(0,nChains),
        [this](const blocked_range<size_t>& range) {
            for (size_t iChain = range.begin(); iChain < range.end(); iChain++)
                this->AdvanceChain( iChain );
        }
    );
#else
    for (size_t iChain = 0; iChain < nChains; iChain++)
        AdvanceChain( iChain );
#endif
}

void MetropolisHastings::AdvanceChain(size_t iChain)
{
    LOG_ASSERT( fProposalFunctions[iChain], "No proposal function defined." );

    auto& chain = fSampledChains[iChain];
    LOG_ASSERT( !chain.empty(), "No starting point in chain " << iChain << "." );

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
        chain.push_back( nextState );
    }
    else {
        nextState = previousState;
        nextState.IncrementGeneration();
        chain.push_back( nextState );
    }
}

} /* namespace vmcmc */
