/**
@file  pathgenerator.cpp
@brief Implementation of correlation handling, common to all path generators
*/

#include <orflib/methods/montecarlo/pathgenerator.hpp>
#include <orflib/math/linalg/linalg.hpp>

BEGIN_NAMESPACE(orf)

void PathGenerator::initCorrelation(Matrix const& corrMat)
{
  if (corrMat.is_empty())
    return;               // no correlation passed, nothing to do
  Matrix fixedCorrel = corrMat;
  spectrunc(fixedCorrel);               // spectral truncation
  choldcmp(fixedCorrel, sqrtCorrel_);   // Cholesky decomposition
}

END_NAMESPACE(orf)
