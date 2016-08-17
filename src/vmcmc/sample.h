/**
 * @file
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 */

#ifndef VMCMC_SAMPLE_H_
#define VMCMC_SAMPLE_H_

#include <vmcmc/blas.h>
#include <vmcmc/numeric.h>

#include <initializer_list>
#include <deque>

namespace vmcmc
{

/**
 * A 'sample' represents a node or data point in a Markov Chain.
 * It holds information about the generation, when it was sampled, the point in
 * the parameter space and the corresponding values for the likelihood,
 * -log(likelihood) and the prior value.
 */
class Sample
{
public:
    Sample(const Vector& pValues);
    Sample(std::initializer_list<double> pValues);
    Sample(size_t nParams = 0);
    virtual ~Sample() { }

    void SetGeneration(size_t value) { fGeneration = value; }
    size_t GetGeneration() const { return fGeneration; }
    size_t IncrementGeneration() { return ++fGeneration; }

    Vector& Values() { return fParameterValues; }
    const Vector& Values() const { return fParameterValues; }
//    size_t size() const { return fParameterValues.size(); }

    /**
     * Reset ::fLikelihood, ::fNegLogLikelihood and ::fPrior to their
     * default values.
     */
    void Reset();

    void SetLikelihood(double value) { fLikelihood = value; }
    double GetLikelihood() const { return fLikelihood; }

    void SetNegLogLikelihood(double value) { fNegLogLikelihood = value; }
    double GetNegLogLikelihood() const { return fNegLogLikelihood; }

    void SetPrior(double value) { fPrior = value; }
    double GetPrior() const { return fPrior; }

private:
    size_t fGeneration;
    Vector fParameterValues;
    double fLikelihood;
    double fNegLogLikelihood;
    double fPrior;
};

using Chain = std::deque<Sample>;

inline Sample::Sample(const Vector& pValues) :
    fGeneration( 0 ),
    fParameterValues( pValues ),
    fLikelihood( 0.0 ),
    fNegLogLikelihood( -Inf() ),
    fPrior( 0.0 )
{ }

inline Sample::Sample(std::initializer_list<double> pValues) :
    fGeneration( 0 ),
    fParameterValues( pValues ),
    fLikelihood( 0.0 ),
    fNegLogLikelihood( -Inf() ),
    fPrior( 0.0 )
{ }

inline Sample::Sample(size_t nParams) :
    fGeneration( 0 ),
    fParameterValues( nParams, 0.0 ),
    fLikelihood( 0.0 ),
    fNegLogLikelihood( -Inf() ),
    fPrior( 0.0 )
{ }

inline void Sample::Reset()
{
    fPrior = fLikelihood = 0.0;
    fNegLogLikelihood = -Inf();
}

} /* namespace vmcmc */

#endif /* VMCMC_SAMPLE_H_ */
