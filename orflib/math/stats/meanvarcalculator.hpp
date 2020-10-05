/**
@file  meanvarcalculator.hpp
@brief Calculates the mean and variance of a set of samples
*/

#ifndef ORF_MEANVARCALCULATOR_HPP
#define ORF_MEANVARCALCULATOR_HPP

#include <orflib/math/stats/statisticscalculator.hpp>
#include <orflib/exception.hpp>

BEGIN_NAMESPACE(orf)

/** Statistics calculator base class */
template <typename ITER>
class MeanVarCalculator : public StatisticsCalculator < ITER >
{
  using StatisticsCalculator<ITER>::nVariables;
  using StatisticsCalculator<ITER>::nsamples_;
  using StatisticsCalculator<ITER>::results_;

public:

  MeanVarCalculator(size_t nvars);

  virtual ~MeanVarCalculator() {}

  virtual void addSample(ITER begin, ITER end) override;

  virtual void reset() override;

  virtual Matrix const & results() override;

protected:

  // state
  Vector runningSum_;
  Vector runningSum2_;

};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

template <typename ITER>
MeanVarCalculator<ITER>::MeanVarCalculator(size_t nvars)
  : StatisticsCalculator<ITER>(nvars, 2), runningSum_(nvars), runningSum2_(nvars)
{
  for (size_t j = 0; j < nvars; ++j) {
    runningSum2_(j) = runningSum_(j) = 0.0;
  }
}

template <typename ITER>
void MeanVarCalculator<ITER>::addSample(ITER begin, ITER end)
{
  ORF_ASSERT(end - begin == nVariables(), "missing variable values!");

  ITER it = begin;
  for (size_t j = 0; j < nVariables(); ++j, ++it) {
    runningSum_(j) += *it;
    runningSum2_(j) += (*it) * (*it);
  }

  ++nsamples_;
}

template <typename ITER>
Matrix const & MeanVarCalculator<ITER>::results()
{
  for (size_t j = 0; j < nVariables(); ++j) {
    results_(0, j) = runningSum_(j) / nsamples_;
    double mean = results_(0, j);
    results_(1, j) = runningSum2_(j) / nsamples_ - mean * mean;
    results_(1, j) *= nsamples_;
    results_(1, j) /= (nsamples_ - 1);
  }

  return results_;
}

template <typename ITER>
void MeanVarCalculator<ITER>::reset()
{
  StatisticsCalculator<ITER>::reset();
  for (size_t j = 0; j < nVariables(); ++j) {
    runningSum_(j) = 0.0;
    runningSum2_(j) = 0.0;
  }
}

END_NAMESPACE(orf)

#endif // ORF_MEANVARCALCULATOR_HPP
