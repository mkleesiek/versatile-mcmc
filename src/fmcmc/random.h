/*
 * random.h
 *
 *  Created on: 27.07.2016
 *      Author: marco@kleesiek.com
 */

#ifndef FMCMC_RANDOM_H_
#define FMCMC_RANDOM_H_

#include <random>
#include <type_traits>
#include <cmath>
#include <mutex>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>

namespace fmcmc
{

namespace ublas = boost::numeric::ublas;

/**
 * A thread-safe interface for the STL random number generators.
 * @tparam EngineT Underlying random number generator (e.g. std::mt19937).
 */
template<class EngineT>
class RandomPrototype
{
public:
    typedef EngineT engine_type;
    typedef typename engine_type::result_type result_type;

public:
    static RandomPrototype& Instance();

protected:
    RandomPrototype(result_type seed = engine_type::default_seed);
    virtual ~RandomPrototype();

    RandomPrototype(const RandomPrototype& other) = delete;
    void operator=(const RandomPrototype& other) = delete;

public:

    /**
     * Get the seed, the random number engine was last initialized with.
     * @return
     */
    result_type GetSeed() const { return fSeed; }

    /**
     * Set the seed on the underlying random number engine.
     * For a seed = 0, the random result of a non-deterministic generator is used.
     * @param seed
     * @return
     */
    result_type SetSeed(result_type seed = engine_type::default_seed);

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
    FloatT Gaussian(FloatT mean = 0.0, FloatT sigma = 1.0);

    /**
     * Draw from a multivariate gaussian.
     * @param mean A vector of mean values.
     * @param cholesky The Cholesky decomposition of the covariance matrix.
     * The diagonal elements correspond to the std. deviations.
     * @return
     * @see boost::multivariate_normal_distribution
     */
    template<class FloatT>
    ublas::vector<FloatT> GaussianMultiVariate(const ublas::vector<FloatT>& mean,
        const ublas::triangular_matrix<FloatT, ublas::lower>& cholesky);

    /**
     * Draw from a multivariate gaussian without correlations.
     * @param mean A vector of mean values.
     * @param sigma A vector of standard deviations.
     * @return
     * @see boost::multivariate_normal_distribution
     */
    template<class FloatT>
    ublas::vector<FloatT> GaussianMultiVariate(ublas::vector<FloatT> mean,
        ublas::vector<FloatT> sigma);

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
     * @tparam The type name of the distribution.
     * @see http://www.cplusplus.com/reference/random/
     * @param dist
     * @return
     */
    template<class DistributionT>
    typename DistributionT::result_type FromDistribution(const DistributionT& dist);

public:
    static constexpr result_type min() { return engine_type::min(); }
    static constexpr result_type max() { return engine_type::max(); }

    /**
     * Invokes the underlying random number generator after locking the mutex.
     * @return
     */
    result_type operator()();

private:
    result_type fSeed;
    engine_type fEngine;
    std::mutex fMtx;
};

template<class EngineT>
inline RandomPrototype<EngineT>& RandomPrototype<EngineT>::Instance()
{
    static RandomPrototype sInstance;
    return sInstance;
}

template<class EngineT>
inline RandomPrototype<EngineT>::RandomPrototype(result_type seed) :
    fSeed(0),
    fEngine()
{
    SetSeed(seed);
}

template<class EngineT>
inline RandomPrototype<EngineT>::~RandomPrototype()
{ }

template<class EngineT>
inline typename RandomPrototype<EngineT>::result_type RandomPrototype<EngineT>::SetSeed(result_type value)
{
    std::lock_guard<std::mutex> lock(fMtx);
    fSeed = (value == 0) ? std::random_device()() : value;
    fEngine.seed(fSeed);
    return fSeed;
}

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
inline FloatT RandomPrototype<EngineT>::Gaussian(FloatT mean, FloatT sigma)
{
    return std::normal_distribution<FloatT>(mean, sigma)(*this);
}

template<class EngineT>
template<class FloatT>
inline ublas::vector<FloatT> RandomPrototype<EngineT>::GaussianMultiVariate(
    const ublas::vector<FloatT>& mean,
    const ublas::triangular_matrix<FloatT, ublas::lower>& cholesky)
{
    assert( mean.size() == cholesky.size1() );

    ublas::vector<FloatT> noise( mean.size() );

    std::normal_distribution<FloatT> dist;
    for (size_t i = 0; i < noise.size(); ++i)
        noise(i) = dist(*this);

    return mean + ublas::prod(noise, cholesky);
}

template<class EngineT>
template<class FloatT>
inline ublas::vector<FloatT> RandomPrototype<EngineT>::GaussianMultiVariate(
    ublas::vector<FloatT> mean,
    ublas::vector<FloatT> sigma)
{
    assert( mean.size() == sigma.size1() );

    ublas::vector<FloatT> noise( mean.size() );

    std::normal_distribution<FloatT> dist;
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
        return Gaussian<FloatT>(mean, sqrt(mean));
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

template<class EngineT>
template<class DistributionT>
inline typename DistributionT::result_type RandomPrototype<EngineT>::FromDistribution(const DistributionT& dist)
{
    return dist(*this);
}

template<class EngineT>
inline typename RandomPrototype<EngineT>::result_type RandomPrototype<EngineT>::operator()()
{
    /*
     * TODO: mutex locks are expensive. think about using a separate generator for
     * each thread -> requires clever seeding.
     */
    std::lock_guard<std::mutex> lock(fMtx);
    return fEngine();
}

using Random = RandomPrototype<std::mt19937>;

} /* namespace fmcmc */

#endif /* FMCMC_RANDOM_H_ */
