/*
 * sample.h
 *
 *  Created on: 05.08.2016
 *      Author: marco@kleesiek.com
 */

#ifndef SRC_FMCMC_SAMPLE_H_
#define SRC_FMCMC_SAMPLE_H_

#include <fmcmc/ublas.h>
#include <boost/optional.hpp>

namespace fmcmc
{

class Sample
{
public:
    Sample(const ublas::vector<double>& pValues) :
        fParameterValues( pValues ) { }
    Sample(size_t nParams = 0) :
        fParameterValues( nParams ) { }
    virtual ~Sample() { }

    ublas::vector<double>& Values() { return fParameterValues; }
    double LikelihoodOr(double defaultValue = 0.0) const;

private:
    ublas::vector<double> fParameterValues;
    boost::optional<double> fLikelihood;
};

inline double Sample::LikelihoodOr(double defaultValue) const
{
    return fLikelihood.get_value_or(defaultValue);
}


} /* namespace fmcmc */

#endif /* SRC_FMCMC_SAMPLE_H_ */
