/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#ifndef FMCMC_ALGORITHM_H_
#define FMCMC_ALGORITHM_H_

#include <fmcmc/parameter.h>
#include <fmcmc/sample.h>

#include <functional>
#include <vector>
#include <deque>
#include <cmath>

namespace fmcmc
{

using Chain = std::deque<Sample>;

/**
 * This base class models the sampling algorithm of an MCMC, advancing to a
 * new state in the parameter space, accepting or rejecting, and saving that
 * state (subsequently sampling the parameter space).
 * Ideally, this interface should cover single chain algorithms, as well as
 * multi-chain algorithms (like DREAM).
 */
class Algorithm
{
public:
    Algorithm();
    virtual ~Algorithm();

    void SetParameterConfig(const ParameterSet& paramConfig);
    const ParameterSet& GetParameterConfig() const { return fParameterConfig; }

    template<class XFunctionT>
    void SetPrior(size_t pIndex, XFunctionT prior);

    template<class XFunctionT>
    void SetLikelihoodFunction(XFunctionT likelihood);

    template<class XFunctionT>
    void SetNegLogLikelihoodFunction(XFunctionT negLoglikelihood);

    void SetTotalLength(size_t length) { fTotalLength = length; }
    size_t GetTotalLength() const { return fTotalLength; }

    void Run();

    virtual bool Initialize() = 0;

    virtual double Advance() = 0;

    virtual size_t NChains() = 0;
    virtual const Chain& GetChain(size_t cIndex = 0) = 0;

protected:
    ParameterSet fParameterConfig;
    std::vector<std::function<double (double)>> fPriors;
    std::function<double (const std::vector<double>&)> fNegLogLikelihood;

    size_t fTotalLength;
};

template<class XFunctionT>
inline void Algorithm::SetPrior(size_t pIndex, XFunctionT prior)
{
    if (fPriors.size() <= pIndex)
        fPriors.resize( pIndex+1, nullptr );

    fPriors[pIndex] = prior;
}

template<class XFunctionT>
inline void Algorithm::SetLikelihoodFunction(XFunctionT likelihood)
{
    fNegLogLikelihood = [likelihood]
        (const std::vector<double>& params) -> double {
            return std::exp(-likelihood(params));
        };
}

template<class XFunctionT>
inline void Algorithm::SetNegLogLikelihoodFunction(XFunctionT negLoglikelihood)
{
    fNegLogLikelihood = negLoglikelihood;
}

} /* namespace fmcmc */

#endif /* FMCMC_ALGORITHM_H_ */
