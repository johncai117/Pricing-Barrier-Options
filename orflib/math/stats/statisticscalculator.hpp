/**
@file  statisticscalculator.hpp
@brief Abstract base class for statistics calculators
*/

#ifndef ORF_STATISTICSCALCULATOR_HPP
#define ORF_STATISTICSCALCULATOR_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>
#include <orflib/math/matrix.hpp>

BEGIN_NAMESPACE(orf)

/** Statistics calculator base class */
template <typename ITER>
class StatisticsCalculator
{
public:

  StatisticsCalculator(size_t nvars, size_t nresults);

  virtual ~StatisticsCalculator() {}

  /** Adds one sample; requires end - big == nVariables() */
  virtual void addSample(ITER begin, ITER end) = 0;

  /** Clears samples and results */
  virtual void reset();

  /** Returns the number of samples addes so far */
  size_t nSamples() const;

  /** Returns the number of variables */
  virtual size_t nVariables() const;

  /** Returns the results, one column per variable */
  virtual Matrix const & results() = 0;

protected:

  // state
  size_t nsamples_;
  mutable Matrix results_;

};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

template <typename ITER>
StatisticsCalculator<ITER>::StatisticsCalculator(size_t nvars, size_t nresults)
  : nsamples_(0), results_(nresults, nvars)
{
  for (size_t i = 0; i < nresults; ++i) {
    for (size_t j = 0; j < nvars; ++j) {
      results_(i, j) = 0.0;
    }
  }
}

template <typename ITER>
size_t StatisticsCalculator<ITER>::nSamples() const
{
  return nsamples_;
}

template <typename ITER>
size_t StatisticsCalculator<ITER>::nVariables() const
{
  return results_.n_cols;
}

template <typename ITER>
void StatisticsCalculator<ITER>::reset()
{
  for (size_t i = 0; i < results_.n_rows; ++i) {
    for (size_t j = 0; j < results_.n_cols; ++j) {
      results_(i, j) = 0.0;
    }
  }
}


END_NAMESPACE(orf)

#endif // ORF_STATISTICSCALCULATOR_HPP
