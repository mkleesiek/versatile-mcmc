/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/metropolis.h>
#include <vmcmc/proposal.h>
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

double MetropolisHastings::Advance()
{
    LOG_ASSERT( fProposalFunction, "No proposal function defined." );

    for (size_t cIndex = 0; cIndex < fSampledChains.size(); cIndex++) {

        auto& chain = fSampledChains[cIndex];
        LOG_ASSERT( !chain.empty(), "No starting point in chain " << cIndex << "." );

        const Sample& currentState = chain.back();
        Sample nextState( currentState.size() );

        fProposalFunction->SetParameterConfig( fDynamicParamConfigs[cIndex] );
        const double proposalAsymmetry = fProposalFunction->Transition( currentState, nextState );

        Evaluate( nextState );
    }

    /*

    // regular MH algorithm

    fNextStateBuffer = fCurrentState;
    const double transRatio = fProposal->Transition(fCurrentState, fNextStateBuffer);

    const double priorCurrent = GetParameterConfig().Prior(fCurrentState);
    const double priorNext = GetParameterConfig().Prior(fNextStateBuffer);

    if (priorCurrent <= 0.0)
        return 1.0;
    else if (priorNext <= 0.0)
        return 0.0;
    else {
        if (GetCurveFitter()->DoesLikelihoodFluctuate())
            UpdateNegLogLikelihood(fCurrentState);

        UpdateNegLogLikelihood(fNextStateBuffer);
        const double mhRatio = min(1.0, transRatio * priorNext/priorCurrent * exp( fBeta * (fCurrentState.GetNegLogLikelihood() - fNextStateBuffer.GetNegLogLikelihood()) ) );

//        if (fDoBurnIn && mhRatio < 1.0)
//            return 0.0;
        return mhRatio;
    }



    assert(GetParameterConfig().Size() == NumberOfParameters() && fSavedStates.Size() > 0 );

    const double acceptanceProb = NextStepProbability();

    const bool accepted = (acceptanceProb >= 1.0 || KRandom::GetInstance().Bool(acceptanceProb));

    if (accepted) {
        fCurrentState = fNextStateBuffer;
//      KDEBUG(i << ": Accepted. p=" << acceptanceProb);
    }
//    else {
//        KDEBUG(i << ": Denied. p=" << acceptanceProb);
//    }

    fCurrentState.IncrementTime();
    fNextStateBuffer.IncrementTime();

    return (accepted) ? 1.0 : 0.0;


    */

    return 0.0;
}

} /* namespace vmcmc */
