/**
@file   eigensym.cpp
@brief  The eigenvalues and eigenvectors of a real symmetric matrix
*/

#include <orflib/math/linalg/linalg.hpp>
#include <orflib/exception.hpp>

BEGIN_NAMESPACE(orf)

void eigensym(Matrix const& inMat, Vector& eigenValues, Matrix& eigenVectors)
{
  ORF_ASSERT(inMat.is_square(), "eigensym: input matrix must be square!");
  arma::eig_sym(eigenValues, eigenVectors, inMat);
  return;
}

END_NAMESPACE(orf)
