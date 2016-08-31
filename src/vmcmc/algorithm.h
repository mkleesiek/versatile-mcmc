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
#include <cmath>

namespace vmcmc
{

class Writer;

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

    /**
     * Set the parameter configuration.
     * This is mandatory for every possible implementations of this class.
     * From the configuration, the start points and proposal functions of a
     * sampler a initialized. The number of parameters defines, with how many
     * arguments the target likelihood function is evaluated.
     * @param paramConfig
     */
    void SetParameterConfig(const ParameterConfig& paramConfig);
    const ParameterConfig& GetParameterConfig() const { return fParameterConfig; }

    template<class FunctionT>
    void SetPrior(FunctionT prior) { fPrior = prior; }

    template<class FunctionT>
    void SetLikelihoodFunction(FunctionT likelihood);
    template<class FunctionT>
    void SetNegLogLikelihoodFunction(FunctionT negLoglikelihood);

    void SetTotalLength(size_t length) { fTotalLength = length; }
    size_t GetTotalLength() const { return fTotalLength; }

    template<class WriterT, class... ArgsT>
    void SetWriter(ArgsT&&... args);
    void SetWriter(std::shared_ptr<Writer> writer) { fWriter = writer; }

    /**
     * Evaluate the prior for the given parameter values.
     * @param paramValues
     * @return The prior value. Returns 1.0 if no priors were defined.
     */
    double EvaluatePrior(const std::vector<double>& paramValues) const;

    /**
     * Evaluate the target function likelihood for the given parameter values.
     * @param paramValues
     * @return The likelihood value.
     */
    double EvaluateLikelihood(const std::vector<double>& paramValues) const;

    /**
     * Evaluate the target function -log(likelihood) for the given parameter values.
     * @param paramValues
     * @return The negative logarithm (natural base) of the likelihood.
     */
    double EvaluateNegLogLikelihood(const std::vector<double>& paramValues) const;

    /**
     * Evalutate the target function prior, likelihood and -log(likelihood)
     * at the position defined by the @p sample, and update the \p sample
     * accordingly.
     * @param sample
     * @return False if the likelihood was not evaluated (e.g. due to a zero
     * prior).
     */
    bool Evaluate(Sample& sample) const;

    /**
     * Start sampling!
     */
    void Run();

    virtual void Initialize();

    virtual void Advance(size_t nSteps = 1) = 0;

    virtual void Finalize();

    virtual size_t NumberOfChains() = 0;
    virtual const Chain& GetChain(size_t cIndex = 0) = 0;

protected:
    ParameterConfig fParameterConfig;
    std::function<double (const std::vector<double>&)> fPrior;

    std::function<double (const std::vector<double>&)> fLikelihood;
    std::function<double (const std::vector<double>&)> fNegLogLikelihood;

    size_t fTotalLength;
    size_t fCycleLength;

    std::shared_ptr<Writer> fWriter;
};

template<class FunctionT>
inline void Algorithm::SetLikelihoodFunction(FunctionT likelihood)
{
    fLikelihood = likelihood;
    fNegLogLikelihood = nullptr;
}

template<class FunctionT>
inline void Algorithm::SetNegLogLikelihoodFunction(FunctionT negLoglikelihood)
{
    fLikelihood = nullptr;
    fNegLogLikelihood = negLoglikelihood;
}

template<class WriterT, class... ArgsT>
inline void Algorithm::SetWriter(ArgsT&&... args)
{
    fWriter = std::make_shared<WriterT>(
        std::forward<ArgsT>(args)...
    );
}

} /* namespace vmcmc */

#endif /* FMCMC_ALGORITHM_H_ */
