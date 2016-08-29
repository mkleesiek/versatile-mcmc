/**
 * @file
 *
 * @date 24.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/random.h>
#include <vmcmc/stringutils.h>
#include <vmcmc/blas.h>

#include <thread>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/covariance.hpp>
#include <boost/accumulators/statistics/variates/covariate.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;
using namespace boost;
using namespace boost::accumulators;

TEST(Random, SingletonInitialization) {

    Random::Seed(1);

    Random& rand = Random::Instance();

    ASSERT_EQ(42, rand.Uniform(0, 100)) << "Deterministic random"
            "number generator with unexpected result.";

    ASSERT_DOUBLE_EQ(43.624248925001041, rand.Uniform(-99.0, +99.0));
}

TEST(Random, UniformMultithreaded) {

    constexpr size_t sNThreads = 4;

    thread myThreads[sNThreads];
    for (size_t t = 0; t < sNThreads; ++t) {
        myThreads[t] = thread( []() {

            accumulator_set<double, stats<tag::mean> > acc;

            for (int i = 0; i < 1000; ++i)
                acc( Random::Instance().Uniform(0.0, 10.0) );

            ASSERT_NEAR(5.0, mean(acc), 0.5);
        });
    }

    for (size_t t = 0; t < sNThreads; ++t) {
        myThreads[t].join();
    }
}

TEST(Random, MultivariateNormal) {

    Random& rand = Random::Instance();

    constexpr size_t N = 5;

    ublas::triangular_matrix<double> cov(N, N);
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            if (i == j)
                cov(i, j) = 1.0 * (double) (i+1);
            else if (i > j)
                cov(i, j) = 0.0;
        }
    }
    cov(1, 0) = 0.8;
    cov(3, 2) = -1.0;
    cov(4, 3) = 3.0;

    ublas::triangular_matrix<double> cholesky(N, N);
    ASSERT_EQ( 0, choleskyDecompose(cov, cholesky) ) << "Cholesky decomposition failed.";

    // TODO: replace boost accumulators by own statistics tools
    accumulator_set<double, stats<tag::mean, tag::variance, tag::covariance<double, tag::covariate1> > > acc01, acc12, acc23, acc34;

    const Vector mean(N, 5.0);

    normal_distribution<double> dist;

    for (int i = 0; i < 1000; ++i) {
        Vector rVector = rand.FromMultiVariateDistribution(dist, mean, cholesky);
        acc01(rVector(0), covariate1 = rVector(1));
        acc12(rVector(1), covariate1 = rVector(2));
        acc23(rVector(2), covariate1 = rVector(3));
        acc34(rVector(3), covariate1 = rVector(4));
    }

    ASSERT_NEAR( 0.8, covariance(acc01), 0.25 );
    ASSERT_NEAR( 0.0, covariance(acc12), 0.30 );
    ASSERT_NEAR( -1.0, covariance(acc23), 0.20 );
    ASSERT_NEAR( 3.0, covariance(acc34), 0.60 );

}
