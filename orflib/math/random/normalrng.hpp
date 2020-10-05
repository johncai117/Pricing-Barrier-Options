/**
@file  normalrng.hpp
@brief Generator of random deviates from the Normal distribution
*/

#ifndef ORF_NORMALRNG_HPP
#define ORF_NORMALRNG_HPP


#include <orflib/defines.hpp>
#include <orflib/exception.hpp>
#include <random>
#include <orflib/math/random/sobolurng.hpp>
#include <orflib/math/stats/normaldistribution.hpp>

BEGIN_NAMESPACE(orf)

/** Generator of normal deviates. It is templatized on the underlying uniform RNG
*/
template<typename URNG>
class NormalRng
{

public:
  /** Ctor from distribution parameters */
  explicit NormalRng(size_t dimension, double mean = 0.0, double stdev = 1.0, URNG const & urng = URNG());

  /** Returns the dimension of the generator */
  size_t dim() const;

  /** Returns a batch of random deviates
      CAUTION: it requires end - begin == dimension() */
  template <typename ITER>
  void next(ITER begin, ITER end);

  /** Returns the underlying uniform rng. */
  URNG & urng();

private:

  // state
  size_t dim_;      // the dimension of the generator
  URNG urng_;       // the uniform random number generator
  std::normal_distribution<double> normcdf_;  // the normal distribution

};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

template<typename URNG>
NormalRng<URNG>::NormalRng(size_t dimension, double mean, double stdev, URNG const & urng)
  : dim_(dimension), urng_(urng)
{
  ORF_ASSERT(stdev > 0.0, "the standard deviation must be positive!");
  normcdf_ = std::normal_distribution<double>(mean, stdev);
}

template<typename URNG>
size_t NormalRng<URNG>::dim() const
{
  return dim_;
}

template<typename URNG>
template <typename ITER>
void NormalRng<URNG>::next(ITER begin, ITER end)
{
  for (ITER it = begin; it != end; ++it)
    *it = normcdf_(urng_);
}

template<typename URNG>
URNG & NormalRng<URNG>::urng()
{
  return urng_;
}

template<>
inline
NormalRng<SobolURng>::NormalRng(size_t dimension, double mean, double stdev, SobolURng const& urng)
: dim_(dimension), urng_(dimension)
{
  ORF_ASSERT(stdev > 0.0, "the standard deviation must be positive!");
  normcdf_ = std::normal_distribution<double>(mean, stdev);
}

template<>
template <typename ITER>
void NormalRng<SobolURng>::next(ITER begin, ITER end)
{
  urng_.next(begin, end);
  orf::NormalDistribution stdnorm;
  for (ITER it = begin; it != end; ++it)
    *it = stdnorm.invcdf(*it);
}

END_NAMESPACE(orf)

#endif // ORF_NORMALRNG_HPP
