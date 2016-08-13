/**
 * @file
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 */

#ifndef VMCMC_SAMPLE_H_
#define VMCMC_SAMPLE_H_

#include <boost/optional.hpp>
#include "blas.h"

namespace vmcmc
{

class Sample
{
public:
    Sample(const Vector& pValues) :
        fParameterValues( pValues ) { }
    Sample(size_t nParams = 0) :
        fParameterValues( nParams ) { }
    virtual ~Sample() { }

    Vector& Values() { return fParameterValues; }
    const Vector& Values() const { return fParameterValues; }

    double GetLikelihoodOr(double defaultValue = 0.0) const;
    void SetLikelihood(const boost::optional<double>& value) { fLikelihood = value; }

private:
    Vector fParameterValues;
    boost::optional<double> fLikelihood;
};

inline double Sample::GetLikelihoodOr(double defaultValue) const
{
    return fLikelihood.get_value_or(defaultValue);
}


} /* namespace vmcmc */

#endif /* VMCMC_SAMPLE_H_ */
