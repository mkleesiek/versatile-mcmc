/*
 * ublas.h
 *
 *  Created on: 29.07.2016
 *      Author: marco@kleesiek.com
 */

#ifndef FMCMC_UBLAS_H_
#define FMCMC_UBLAS_H_

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_expression.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include <boost/numeric/ublas/triangular.hpp>

namespace fmcmc {

namespace ublas = boost::numeric::ublas;

/**
 * Decompose a symmetric positive definit matrix A into product L L^T.
 * @param A A Square symmetric positive definit input matrix. Only the
 * lower triangle is accessed.
 * @param L Lower triangular output matrix, the Cholesky decomposition.
 * @return Nonzero if decomposition fails (then the value is 1 + the number
 * of the failing row)
 */
template <class InputMatrix, class OutputTriangularMatrix>
size_t choleskyDecompose(const InputMatrix& A, OutputTriangularMatrix& L)
{
    using namespace boost::numeric::ublas;

    assert(A.size1() == A.size2());
    assert(L.size1() == L.size2());
    assert(A.size1() == L.size1());

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

            matrix_column < OutputTriangularMatrix > cLk(L, k);
            project(cLk, range(k + 1, n)) = (project(column(A, k),
                range(k + 1, n)) - prod(project(L, range(k + 1, n), range(0, k)),
                project(row(L, k), range(0, k)))) / L_kk;
        }
    }
    return 0;
}

} /* namespace fmcmc */

#endif /* FMCMC_UBLAS_H_ */
