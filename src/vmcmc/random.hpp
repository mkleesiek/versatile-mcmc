/**
 * @file
 *
 * @date 27.07.2016
 * @author marco@kleesiek.com
 *
 * @brief Contains the prototype class for thread-safe random number generators.
 */

#ifndef VMCMC_RANDOM_H_
#define VMCMC_RANDOM_H_

#include <vmcmc/logger.hpp>

#include <random>
#include <type_traits>
#include <cmath>
#include <atomic>
#include <thread>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>

namespace vmcmc
{

namespace ublas = boost::numeric::ublas;

/**
 * A thread-safe interface for STL style pesudo random number generators (PRNG).
 * Each thread accesses it's own static instance of a PRNG, which is
 * realized using the thread_local keyword.
 * In order for each new PRNG instance to be constructed with a new seed, a
 * global variable Random::sSeed (initially to be set with Random::Seed),
 * is incremented and used in each PRNG's constructor.
 * @tparam EngineT Underlying random number generator (e.g. std::mt19937).
 */
template<class EngineT>
class RandomPrototype
{
public:
    typedef EngineT engine_type;
    typedef typename engine_type::result_type result_type;

    /**
     * Set the initial value for the global seed variable.
     * For seed = 0, a non-deterministic value is generated.
     * @param seed
     */
    static void Seed(result_type seed);

    /**
     * Get a reference to a static instance for the current thread.
     * @return
     */
    static RandomPrototype& Instance();

protected:
    static std::atomic<result_type> sSeed;

    RandomPrototype();
    virtual ~RandomPrototype();

    RandomPrototype(const RandomPrototype& other) = delete;
    RandomPrototype(RandomPrototype&& other) = delete;
    void operator=(const RandomPrototype& other) = delete;
    void operator=(RandomPrototype&& other) = delete;

public:
    /**
     * Get a reference to the underlying random number engine (mersenne twister).
     * @return
     */
    engine_type& GetEngine() { return fEngine; }

    /**
     * Get a random uniform number in the specified range.
     * @param min lower interval bound
     * @param max upper interval bound
     * @param minIncluded Include the lower bound.
     * @param maxIncluded Include the upper bound.
     *
     * @return
     */
    template<class FloatT>
    typename std::enable_if<std::is_floating_point<FloatT>::value, FloatT>::type
    Uniform(FloatT min, FloatT max, bool minIncluded, bool maxIncluded);

    /**
     * Get a random uniform number in the specified range [min, max).
     * @param min lower interval bound
     * @param max upper interval bound
     * @return
     */
    template<class FloatT = double>
    typename std::enable_if<std::is_floating_point<FloatT>::value, FloatT>::type
    Uniform(FloatT min = 0.0, FloatT max = 1.0);

    /**
     * Get a random uniform number from a discrete distribution [inclMin, inclMax].
     * @param inclMin
     * @param inclMax
     * @return
     */
    template<class IntegerT>
    typename std::enable_if<std::is_integral<IntegerT>::value, IntegerT>::type
    Uniform(IntegerT inclMin, IntegerT inclMax);

    /**
     * Return a boolean.
     * @param probability
     * @return True with the given probability.
     */
    template<class FloatT = double>
    bool Bool(FloatT probability = 0.5);

    /**
     * Draw from a gaussian / normal distribution.
     * @param mean
     * @param sigma
     * @return
     */
    template<class FloatT = double>
    FloatT Normal(FloatT mean = 0.0, FloatT sigma = 1.0);

    template<class FloatT = double>
    FloatT StudentT(FloatT n = 1.0, FloatT mean = 0.0, FloatT sigma = 1.0);

    /**
     * Draw from an exponential distribution according to exp(-t/tau).
     * @param tau
     * @return
     */
    template<class FloatT>
    inline FloatT Exponential(FloatT tau);

    /**
     * Draw an integer value from a poisson distribution.
     * @param mean
     * @return
     */
    template<class IntegerT = uint32_t>
    typename std::enable_if<std::is_integral<IntegerT>::value, IntegerT>::type
    Poisson(double mean);

    /**
     * Draw a float value (cast from integer) from a poisson distribution.
     * @param mean
     * @return
     */
    template<class FloatT>
    typename std::enable_if<std::is_floating_point<FloatT>::value, FloatT>::type
    Poisson(FloatT mean);

    /**
     * Produces integers in the range [0, n) with the probability of producing each value is specified by the parameters of the distribution.
     * @return
     */
    template<class ProbRangeT, class IndexType = uint32_t>
    IndexType Discrete(const ProbRangeT& probabilities);

    /**
     * Draw from a custom distribution.
     * @tparam Typename of the distribution.
     * @see http://www.cplusplus.com/reference/random/
     * @param dist
     * @return
     */
    template<class DistributionT>
    typename DistributionT::result_type FromDistribution(DistributionT& dist = DistributionT());

    /**
     * Draw from a custom multivariate distribution.
     * @param dist
     * @param mean A vector of mean values.
     * @param cholesky The lower triangular matrix cholesky decomposition of
     * the covariance matrix. The diagonal elements correspond to the
     * individual parameter std. deviations.
     * @return
     * @see boost::multivariate_normal_distribution
     */
    template<class DistributionT, class VectorT, class MatrixT>
    VectorT FromMultiVariateDistribution(DistributionT& dist, const VectorT& mean, const MatrixT& cholesky);

    /**
     * Draw from a custom multivariate distribution without correlations.
     * @param dist
     * @param mean A vector of mean values.
     * @param sigma A vector of standard deviations.
     * @return
     */
    template<class DistributionT, class VectorT>
    VectorT FromMultiVariateDistribution(DistributionT& dist, const VectorT& mean, const VectorT& sigma);

public:
    static constexpr result_type min() { return engine_type::min(); }
    static constexpr result_type max() { return engine_type::max(); }

    /**
     * Invokes the underlying random number generator.
     * @return
     */
    result_type operator()();

private:
    engine_type fEngine;
};

template<class EngineT>
void RandomPrototype<EngineT>::Seed(result_type seed)
{
    sSeed = (seed == 0) ? std::random_device()() : seed;
}

template<class EngineT>
std::atomic<typename EngineT::result_type> RandomPrototype<EngineT>::sSeed( 0 );



// TODO: Alright, Xcode as of version 7.3 does not support thread_local.
// At least, non-POD types are not supported.
// Possible workarounds: use mutexes or boost::thread_specific_ptr.
// For now I use a mutex lock, if thread_local is not supported by the compiler.

#ifdef NO_TLS

#include <mutex>

template<class EngineT>
inline RandomPrototype<EngineT>& RandomPrototype<EngineT>::Instance()
{
    static RandomPrototype sInstance;
    return sInstance;
}

template<class EngineT>
inline auto RandomPrototype<EngineT>::operator()() -> result_type
{
    static std::mutex sMtx;
    std::lock_guard<std::mutex> lock(sMtx);

    return fEngine();
}

#else

template<class EngineT>
inline RandomPrototype<EngineT>& RandomPrototype<EngineT>::Instance()
{
    static thread_local RandomPrototype sInstance;
    return sInstance;
}

template<class EngineT>
inline auto RandomPrototype<EngineT>::operator()() -> result_type
{
    return fEngine();
}

#endif // NO_TLS


template<class EngineT>
inline RandomPrototype<EngineT>::RandomPrototype() :
    fEngine()
{
    fEngine.seed( sSeed++ );
}

template<class EngineT>
inline RandomPrototype<EngineT>::~RandomPrototype()
{ }

template<class EngineT>
template<class FloatT>
typename std::enable_if<std::is_floating_point<FloatT>::value, FloatT>::type
inline RandomPrototype<EngineT>::Uniform(FloatT min, FloatT max, bool minIncluded, bool maxIncluded)
{
    if (minIncluded) {
        if (!maxIncluded) {
            // pass
        }
        else {
            max = std::nextafter(max, (max > min) ? std::numeric_limits<FloatT>::max() : -std::numeric_limits<FloatT>::max());
        }
    }
    else {
        if (!maxIncluded) {
            min = std::nextafter(min, (max > min) ? std::numeric_limits<FloatT>::max() : -std::numeric_limits<FloatT>::max());
        }
        else {
            std::swap(min, max);
        }
    }
    return std::uniform_real_distribution<FloatT>(min, max)(*this);
}

template<class EngineT>
template<class FloatT>
typename std::enable_if<std::is_floating_point<FloatT>::value, FloatT>::type
inline RandomPrototype<EngineT>::Uniform(FloatT min, FloatT max)
{
    return std::uniform_real_distribution<FloatT>(min, max)(*this);
}

template<class EngineT>
template<class IntegerT>
typename std::enable_if<std::is_integral<IntegerT>::value, IntegerT>::type
inline RandomPrototype<EngineT>::Uniform(IntegerT inclMin, IntegerT inclMax)
{
    return std::uniform_int_distribution<IntegerT>(inclMin, inclMax)(*this);
}

template<class EngineT>
template<class FloatT>
inline bool RandomPrototype<EngineT>::Bool(FloatT probability)
{
    return std::uniform_real_distribution<FloatT>(0.0, 1.0)(*this) < probability;
}

template<class EngineT>
template<class FloatT>
inline FloatT RandomPrototype<EngineT>::Normal(FloatT mean, FloatT sigma)
{
    return std::normal_distribution<FloatT>(mean, sigma)(*this);
}

template<class EngineT>
template<class DistributionT>
inline typename DistributionT::result_type RandomPrototype<EngineT>::FromDistribution(DistributionT& dist)
{
    return dist(*this);
}

template<class EngineT>
template<class DistributionT, class VectorT, class MatrixT>
inline VectorT RandomPrototype<EngineT>::FromMultiVariateDistribution(DistributionT& dist, const VectorT& mean, const MatrixT& cholesky)
{
    LOG_DEFINE("vmcmc.random");
    LOG_ASSERT( mean.size() == cholesky.size1() );

    VectorT noise( mean.size() );

    for (size_t i = 0; i < noise.size(); ++i)
        noise[i] = dist(*this);

    return mean + ublas::prod(noise, cholesky);
}

template<class EngineT>
template<class DistributionT, class VectorT>
inline VectorT RandomPrototype<EngineT>::FromMultiVariateDistribution(DistributionT& dist, const VectorT& mean, const VectorT& sigma)
{
    LOG_DEFINE("vmcmc.random");
    LOG_ASSERT( mean.size() == sigma.size1() );

    VectorT noise( mean.size() );

    for (size_t i = 0; i < noise.size(); ++i)
        noise[i] = dist(*this);

    return mean + ublas::element_prod(noise, sigma);
}


template<class EngineT>
template<class IntegerT>
typename std::enable_if<std::is_integral<IntegerT>::value, IntegerT>::type
inline RandomPrototype<EngineT>::Poisson(double mean)
{
    return std::poisson_distribution<IntegerT>(mean)(*this);
}

template<class EngineT>
template<class FloatT>
typename std::enable_if<std::is_floating_point<FloatT>::value, FloatT>::type
inline RandomPrototype<EngineT>::Poisson(FloatT mean)
{
    if (mean > std::numeric_limits<uint64_t>::max() / 2.0)
        return Normal<FloatT>(mean, sqrt(mean));
    else
        return (FloatT) std::poisson_distribution<uint64_t>(mean)(*this);
}

template<class EngineT>
template<class FloatT>
inline FloatT RandomPrototype<EngineT>::Exponential(FloatT tau)
{
    return std::exponential_distribution<FloatT>(1.0/tau)(*this);
}

template<class EngineT>
template<class ProbRangeT, class IndexType>
inline IndexType RandomPrototype<EngineT>::Discrete(const ProbRangeT& probabilities)
{
    return std::discrete_distribution<IndexType>(probabilities.begin(), probabilities.end())(*this);
}


/**
 * Typedef for the default random number generator, based on the Mersenne Twister.
 */
using Random = RandomPrototype<std::mt19937>;

} /* namespace vmcmc */

#endif /* VMCMC_RANDOM_H_ */
