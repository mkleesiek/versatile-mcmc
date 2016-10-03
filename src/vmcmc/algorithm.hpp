/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 *
 * @brief Contains abstract base class for MCMC sampling algorithms.
 */

#ifndef VMCMC_ALGORITHM_H_
#define VMCMC_ALGORITHM_H_

#include <vmcmc/parameter.hpp>
#include <vmcmc/chain.hpp>
#include <vmcmc/typetraits.hpp>

#include <functional>
#include <vector>
#include <cmath>

namespace vmcmc
{

class Writer;

/**
 * Abstract base class for the core MCMC sampling algorithms.
 *
 * This base class models the sampling algorithm of an MCMC, advancing to a
 * new state in the parameter space, accepting or rejecting, and saving that
 * state (subsequently sampling the parameter space).
 *
 * Ideally, this interface should cover single chain algorithms, as well as
 * multi-chain algorithms (like DREAM).
 */
class Algorithm
{
public:
    using DefaultCallable = std::function<double (const std::vector<double>&)>;

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

    void SetPrior(DefaultCallable prior);
    template <size_t NParams, typename CallableT>
    void SetPrior(CallableT prior);

    void SetLikelihood(DefaultCallable likelihood);
    template <size_t NParams, typename CallableT>
    void SetLikelihood(CallableT likelihood);

    void SetNegLogLikelihood(DefaultCallable negLoglikelihood);
    template <size_t NParams, typename CallableT>
    void SetNegLogLikelihood(CallableT negLoglikelihood);

    void SetTotalLength(size_t length) { fTotalLength = length; }
    size_t GetTotalLength() const { return fTotalLength; }

    /**
     * Add an output writer by specifying its type and passing constructor
     * arguments.
     * @param args
     */
    template <typename WriterT, typename... ArgsT>
    void AddWriter(ArgsT&&... args);
    void AddWriter(std::shared_ptr<Writer> writer) { fWriters.push_back( writer ); }

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

    ChainSetStatistics& GetStatistics() { return fStatistics; }
    const ChainSetStatistics& GetStatistics() const { return fStatistics; }

protected:
    ParameterConfig fParameterConfig;
    DefaultCallable fPrior;

    DefaultCallable fLikelihood;
    DefaultCallable fNegLogLikelihood;

    size_t fTotalLength;
    size_t fCycleLength;

    std::vector<std::shared_ptr<Writer>> fWriters;

    ChainSetStatistics fStatistics;
};

namespace detail
{

/**
 * Call a variadic function using a vector of arguments.
 * The vector is recursively packed into a variadic argument pack.
 * Inspired by http://stackoverflow.com/a/23954589/6908762.
 * @param NParams The number of arguments.
 * @param f The target function to call.
 * @param v The argument vector.
 * @param ts Used internally for packing.
 * @return The target callable result.
 */
template <size_t NParams, typename C, typename... Ts>
typename std::enable_if<NParams == sizeof...(Ts), double>::type
inline apply_first_n(C f, const std::vector<double> &v, Ts&&... ts)
{
    return f(std::forward<Ts>(ts)...);
}

/**
 * @overload
 */
template <size_t NParams, typename C, typename... Ts>
typename std::enable_if<NParams != sizeof...(Ts), double>::type
inline apply_first_n(C f, const std::vector<double> &v, Ts&&... ts)
{
    constexpr ptrdiff_t index = NParams - sizeof...(Ts) - 1;
    static_assert(index >= 0, "invalid number of function parameters");
    return apply_first_n<NParams>(f, v, *(std::begin(v) + index), std::forward<Ts>(ts)...);
}

} /* namespace detail */

inline void Algorithm::SetPrior(DefaultCallable prior)
{
    fPrior = prior;
}

template <size_t NParams, typename CallableT>
inline void Algorithm::SetPrior(CallableT f)
{
    fPrior = [=](const std::vector<double>& v) {
        return detail::apply_first_n<NParams>(f, v);
    };
}

inline void Algorithm::SetLikelihood(DefaultCallable likelihood)
{
    fLikelihood = likelihood;
    fNegLogLikelihood = nullptr;
}

template <size_t NParams, typename CallableT>
inline void Algorithm::SetLikelihood(CallableT f)
{
    fLikelihood = [=](const std::vector<double>& v) {
        return detail::apply_first_n<NParams>(f, v);
    };
    fNegLogLikelihood = nullptr;
}

inline void Algorithm::SetNegLogLikelihood(DefaultCallable negLoglikelihood)
{
    fLikelihood = nullptr;
    fNegLogLikelihood = negLoglikelihood;
}

template <size_t NParams, typename CallableT>
inline void Algorithm::SetNegLogLikelihood(CallableT f)
{
    fLikelihood = nullptr;
    fNegLogLikelihood = [=](const std::vector<double>& v) {
        return detail::apply_first_n<NParams>(f, v);
    };
}

template <typename WriterT, typename... ArgsT>
inline void Algorithm::AddWriter(ArgsT&&... args)
{
    fWriters.emplace_back( std::make_shared<WriterT>(std::forward<ArgsT>(args)...) );
}

} /* namespace vmcmc */

#endif /* VMCMC_ALGORITHM_H_ */
