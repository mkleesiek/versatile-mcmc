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

using namespace std;

namespace vmcmc
{

LOG_DEFINE("vmcmc.metropolis");

MetropolisHastings::MetropolisHastings() :
    fRandomizeStartPoint( false ),
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
    fDynamicParamConfigs.assign( nChains, fParameterConfig );

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

    if (!fProposalFunction)
        fProposalFunction.reset( new ProposalGaussian() );

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

double MetropolisHastings::Advance()
{
    LOG_ASSERT( fProposalFunction, "No proposal function defined." );

    bool chain0Accepted = false;

    for (size_t cIndex = 0; cIndex < fSampledChains.size(); cIndex++) {

        auto& chain = fSampledChains[cIndex];
        LOG_ASSERT( !chain.empty(), "No starting point in chain " << cIndex << "." );

        const Sample& previousState = chain.back();

        // prepare the upcoming sample
        Sample nextState( previousState );
        nextState.IncrementGeneration();
        nextState.Reset();

        // propose the next point in the parameter space
        fProposalFunction->SetParameterConfig( fDynamicParamConfigs[cIndex] );
        const double proposalAsymmetry = fProposalFunction->Transition( previousState, nextState );

        // attempt reflection if limits are exceeded
        fDynamicParamConfigs[cIndex].ReflectFromLimits( nextState.Values() );

        // evaluate likelihood and prior
        Evaluate( nextState );

        const double mhRatio = CalculateMHRatio(previousState, nextState, proposalAsymmetry, fBetas[cIndex]);

        const bool proposalAccepted = Random::Instance().Bool( mhRatio );

        if (proposalAccepted) {
            chain.push_back( nextState );
            if (cIndex == 0)
                chain0Accepted = true;
        }
        else {
            nextState = previousState;
            nextState.IncrementGeneration();
            chain.push_back( nextState );
        }
    }

    return (chain0Accepted) ? 1.0 : 0.0;
}

} /* namespace vmcmc */
