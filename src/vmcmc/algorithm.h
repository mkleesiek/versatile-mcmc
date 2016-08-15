/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 */

#ifndef FMCMC_ALGORITHM_H_
#define FMCMC_ALGORITHM_H_

#include <vmcmc/parameter.h>
#include <vmcmc/sample.h>

#include <functional>
#include <vector>
#include <deque>
#include <cmath>

namespace vmcmc
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

    template<class FunctionT>
    void SetPrior(FunctionT prior) { fPrior = prior; }

    template<class FunctionT>
    void SetLikelihoodFunction(FunctionT likelihood);
    template<class FunctionT>
    void SetNegLogLikelihoodFunction(FunctionT negLoglikelihood);

    void SetTotalLength(size_t length) { fTotalLength = length; }
    size_t GetTotalLength() const { return fTotalLength; }

    /**
     * Evaluates the likelihood or -log(likelihood) function for the given
     * parameter values.
     * @param[in] paramValues
     * @param[out] likelihood
     * @param[out] negLogLikelihood
     * @param[out] prior
     */
    void Evaluate(const std::vector<double>& paramValues,
        double& likelihood, double& negLogLikelihood, double& prior) const;

    void Run();

    virtual bool Initialize();

    virtual double Advance() = 0;

    virtual size_t NChains() = 0;
    virtual const Chain& GetChain(size_t cIndex = 0) = 0;

protected:
    void Evaluate(Sample& sample) const;

    ParameterSet fParameterConfig;
    std::function<double (const std::vector<double>&)> fPrior;

    std::function<double (const std::vector<double>&)> fLikelihood;
    std::function<double (const std::vector<double>&)> fNegLogLikelihood;

    size_t fTotalLength;
};

template<class FunctionT>
inline void Algorithm::SetLikelihoodFunction(FunctionT likelihood)
{
//    fNegLogLikelihood = [likelihood]
//        (const std::vector<double>& params) -> double {
//            return std::exp(-likelihood(params));
//        };
    fLikelihood = likelihood;
    fNegLogLikelihood = nullptr;
}

template<class FunctionT>
inline void Algorithm::SetNegLogLikelihoodFunction(FunctionT negLoglikelihood)
{
    fLikelihood = nullptr;
    fNegLogLikelihood = negLoglikelihood;
}

} /* namespace vmcmc */

#endif /* FMCMC_ALGORITHM_H_ */
