/**
 * @file
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 */

#ifndef VMCMC_SAMPLE_H_
#define VMCMC_SAMPLE_H_

#include <vmcmc/blas.h>

namespace vmcmc
{

class Sample
{
public:
    Sample(const Vector& pValues) :
        fParameterValues( pValues ), fLikelihood( 0.0 ), fPrior( 0.0 ) { }
    Sample(size_t nParams = 0) :
        fParameterValues( nParams ), fLikelihood( 0.0 ), fPrior( 0.0 ) { }
    virtual ~Sample() { }

    Vector& Values() { return fParameterValues; }
    const Vector& Values() const { return fParameterValues; }

    void SetLikelihood(double value) { fLikelihood = value; }
    double GetLikelihood() const { return fLikelihood; }

    void SetPrior(double value) { fPrior = value; }
    double GetPrior() const { return fPrior; }

    size_t size() const { return fParameterValues.size(); }

private:
    Vector fParameterValues;
    double fLikelihood;
    double fPrior;
};

} /* namespace vmcmc */

#endif /* VMCMC_SAMPLE_H_ */
