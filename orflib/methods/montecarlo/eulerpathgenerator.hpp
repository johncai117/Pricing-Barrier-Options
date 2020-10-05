/**
@file  pathgenerator.hpp
@brief Definition of Monte Carlo path generator with Euler time stepping
*/

#ifndef ORF_EULERPATHGENERATOR_HPP
#define ORF_EULERPATHGENERATOR_HPP

#include <orflib/methods/montecarlo/pathgenerator.hpp>
#include <orflib/math/random/rng.hpp>

BEGIN_NAMESPACE(orf)

/** Creates standard normal increments populating the time line sequentially.
    It is templetized on the underlying normal deviate generator.
*/
template <typename NRNG>
class EulerPathGenerator : public PathGenerator
{
public:

  /** Ctor for generating increments for correlated factors.
      If the correlation matrix is not passed in, it assumes independent factors
  */
  template<typename ITER>
  EulerPathGenerator(ITER timestepsBegin, ITER timestepsEnd, size_t nfactors,
                     Matrix const & correlMat = Matrix());

  /** Returns the dimension of the generator */
  size_t dim() const;

  /** Returns the next price path */
  virtual void next(Matrix& pricePath) override;

protected:
  NRNG nrng_;
  Vector sqrtDeltaT_;              // sqrt(T1), sqrt(T2-T1), ...
  Vector normalDevs_;              // scratch array

};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

template <typename NRNG>
template <typename ITER>
inline EulerPathGenerator<NRNG>::EulerPathGenerator(ITER timestepsBegin,
                          ITER timestepsEnd,
                          size_t nfactors,
                          Matrix const& correlMat)
  : PathGenerator((timestepsEnd - timestepsBegin), nfactors, correlMat),
  nrng_((timestepsEnd - timestepsBegin) * nfactors, 0.0, 1.0)
{
  ORF_ASSERT(ntimesteps_ > 0, "no time steps!");
  normalDevs_.resize(ntimesteps_);
  sqrtDeltaT_.resize(ntimesteps_);
  sqrtDeltaT_[0] = sqrt(*timestepsBegin);
  ITER it = ++timestepsBegin;
  size_t i = 1;
  for (; it != timestepsEnd; ++it, ++i) {
    double deltaT = *it - *(it - 1);
    ORF_ASSERT(deltaT > 0.0, "time steps are not unique or not in increasing order!");
    sqrtDeltaT_(i) = sqrt(deltaT);
  }
}

template <typename NRNG>
inline size_t EulerPathGenerator<NRNG>::dim() const
{
  return nrng_.dim();
}

template <typename NRNG>
inline void EulerPathGenerator<NRNG>::next(Matrix& pricePath)
{
  pricePath.resize(ntimesteps_, nfactors_);
  // iterate over columns; the matrix will be filled column by column
  for (size_t j = 0; j < nfactors_; ++j) {
    nrng_.next(normalDevs_.begin(), normalDevs_.end());
    for (size_t i = 0; i < ntimesteps_; ++i)
      pricePath(i, j) = normalDevs_(i);
  }
  // finally apply the Cholesky factor if not empty
  if (sqrtCorrel_.n_rows != 0) {
    for (size_t i = 0; i < ntimesteps_; ++i) {
      for (size_t j = 0; j < nfactors_; ++j) {
        double sum = 0.0;
        for (size_t k = 0; k < nfactors_; ++k) {
          sum += sqrtCorrel_(nfactors_ - j - 1, k) * pricePath(i, k);
        }
        pricePath(i, nfactors_ - j - 1) = sum;
      }
    }
  }
}

END_NAMESPACE(orf)

#endif // ORF_EULERPATHGENERATOR_HPP
