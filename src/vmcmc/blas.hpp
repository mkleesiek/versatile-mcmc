/**
 * @file
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 *
 * @brief BLAS (Basic Linear Algebra Subprograms) utility functions.
 */

#ifndef VMCMC_BLAS_H_
#define VMCMC_BLAS_H_

#include <vmcmc/logger.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_expression.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include <boost/numeric/ublas/triangular.hpp>

namespace vmcmc {

namespace ublas = boost::numeric::ublas;

using Vector          = ublas::vector<double, std::vector<double>>;
using Matrix          = ublas::matrix<double, ublas::row_major, std::vector<double>>;
using MatrixLower     = ublas::triangular_matrix<double, ublas::lower, ublas::row_major, std::vector<double>>;
using MatrixUnitLower = ublas::triangular_matrix<double, ublas::unit_lower, ublas::row_major, std::vector<double>>;

/**
 * Decompose a symmetric positive definit matrix A into product L L^T.
 * @param A A Square symmetric positive definit input matrix. Only the
 * lower triangle is accessed.
 * @param L Lower triangular output matrix, the Cholesky decomposition.
 * @return Nonzero if decomposition fails (then the value is 1 + the number
 * of the failing row)
 */
template <class InputMatrix, class OutputTriangularMatrix>
inline size_t choleskyDecompose(const InputMatrix& A, OutputTriangularMatrix& L)
{
    LOG_DEFINE("vmcmc.blas");
    using namespace boost::numeric::ublas;

    LOG_ASSERT(A.size1() == A.size2());
    LOG_ASSERT(L.size1() == L.size2());
    LOG_ASSERT(A.size1() == L.size1());

    const size_t n = A.size1();

    for (size_t k = 0; k < n; k++) {

        double qL_kk = A(k, k)
                - inner_prod(project(row(L, k), range(0, k)),
                        project(row(L, k), range(0, k)));

        if (qL_kk <= 0) {
            return 1 + k;
        }
        else {
            double L_kk = sqrt(qL_kk);
            L(k, k) = L_kk;
            matrix_column<OutputTriangularMatrix> cLk(L, k);

            project(cLk, range(k + 1, n)) = (project(column(A, k),
                range(k + 1, n)) - prod(project(L, range(k + 1, n), range(0, k)),
                project(row(L, k), range(0, k)))) / L_kk;
        }
    }
    return 0;
}

inline bool operator== (const Vector& v1, const Vector& v2)
{
    return v1.data() == v2.data();
}

inline bool operator!= (const Vector& v1, const Vector& v2)
{
    return v1.data() != v2.data();
}

} /* namespace vmcmc */

#endif /* VMCMC_BLAS_H_ */
