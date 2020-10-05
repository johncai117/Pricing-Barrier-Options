/**
@file   choldcmp.cpp
@brief  Implementation of the Cholesky decomposition
*/

#include <orflib/math/linalg/linalg.hpp>
#include <orflib/exception.hpp>

BEGIN_NAMESPACE(orf)

/** Cholesky decomposition of a positive semi-definite matrix inMat.
  The returned matrix is lower triangular, such that 
  outMat * outMat^T = inMat
*/
void choldcmp(Matrix const& inMat, Matrix& outMat)
{
  ORF_ASSERT(inMat.is_square(), "choldcmp: input matrix must be square!");
  ORF_ASSERT(arma::approx_equal(inMat, inMat.t(), "absdiff", 1.0e-16), "choldcmp: input matrix must be symmetric!");
  bool  ok = arma::chol(outMat, inMat, "lower");
  ORF_ASSERT(ok, "choldcmp: input matrix not positive definite!");
  return;
}

END_NAMESPACE(orf)
