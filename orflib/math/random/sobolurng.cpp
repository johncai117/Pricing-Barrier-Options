/**
    @file  sobolurng.cpp
    @brief Implementation of the Sobol sequence generator
*/

#include <orflib/math/random/sobolurng.hpp>
#include <orflib/math/random/primitivepolynomials.hpp>
#include <cstddef>

BEGIN_NAMESPACE(orf)

long SobolURng::polyInit(long dimension)
{
  ORF_ASSERT(dimension <= MAX_PRIMITIVEPOLY, "too many dimensions in Sobol URNG");

  long dimCount = 1;
  long degCount = 1;
  long curCount = 0;

  while (dimCount <= dimension && degCount <= MAX_PRIMITIVEDEGREE) {
    if (PrimitivePolynomials[degCount - 1][curCount] < 0) {
      ++degCount;
      curCount = 0;
    }

    otpol[dimCount - 1] = PrimitivePolynomials[degCount - 1][curCount];
    deg[dimCount - 1] = degCount;

    // loop
    ++curCount;
    ++dimCount;
  }
  return degCount;
}


long SobolURng::ivInit(long dimension)
{
  // initialize the polynomials
  long maxdeg = polyInit(dimension);

  if (!maxdeg)
    return 0;

  // allocate the vector of the initial values
  iv = (long*)calloc(dimension * MAXBIT, sizeof(long));
  if (iv == NULL)
    return 0;

  // loop over bits up to maxdeg and for each dimension set the initial values
  long lim = 2;
  for (long j = 0; j < maxdeg; ++j) {
    long val = 1;
    for (long k = 0; k < dimension; ++k) {
      val += 2;  // val ranges over the odd numbers
      iv[dimension * j + k] = val % lim;
    }
    lim <<= 1;  // times 2
  }
  return maxdeg;
}


void SobolURng::init(size_t dimension)
{
  // initialize polynomials and initial values
  long memerror = ivInit((long) dimension);
  if (!memerror) return;

  // set vector iu to provide 2D access into iv
  for (size_t j = 0, k = 0; j < MAXBIT; j++, k += dimension)
    iu[j] = &iv[k];

  for (size_t k = 0; k < dimension; k++) {
    long poldeg = deg[k];

    for (size_t j = 0; j < (size_t) poldeg; j++)
      iu[j][k] <<= (MAXBIT - j -1);

    for (size_t j = poldeg; j < MAXBIT; j++) {
      long ipp = otpol[k];
      long i = iu[j - poldeg][k];
      i ^= (i >> poldeg);

      for (ptrdiff_t l = poldeg - 1; l >= 1; l--) {
        if (ipp & 1) i ^= iu[j - l][k];
        ipp >>= 1;
      }
      iu[j][k] = i;
    }
  }

  // the otpol and deg vectors are not needed any more
  // we could reclaim their storage.
}

END_NAMESPACE(orf)
