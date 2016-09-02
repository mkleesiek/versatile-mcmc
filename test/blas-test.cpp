/**
 * @file
 *
 * @date 24.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/blas.h>
#include <vmcmc/stringutils.h>

#include <boost/numeric/ublas/triangular.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(BLAS, CholeskyDecomposition) {

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
    cov(3, 2) = -1.5;
    cov(4, 3) = 3.0;

    ublas::triangular_matrix<double> cholesky(N, N);
    ASSERT_EQ( 0, choleskyDecompose(cov, cholesky) ) << "Cholesky decomposition failed.";

    ASSERT_EQ( N, cholesky.size1() );
    ASSERT_EQ( cholesky.size2(), cholesky.size1() );

    const ublas::matrix<double> covTest = ublas::prod(cholesky, ublas::trans(cholesky));

    ASSERT_EQ( N, covTest.size1() );
    ASSERT_EQ( covTest.size2(), covTest.size1() );

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j <= i; ++j) {
            // test symmetry
            ASSERT_DOUBLE_EQ( covTest(i, j), covTest(j, i));
            // test reversal of cholesky decomp.
            ASSERT_DOUBLE_EQ( covTest(i, j), cov(i, j) )
                << "Cholesky decomposition seems faulty for element(" << i << ", " << j << ").";
        }
    }
}
