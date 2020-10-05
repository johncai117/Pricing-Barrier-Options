/**
@file  normaldistribution.hpp
@brief The normal distribution and its inverse
*/

#ifndef ORF_NORMALDISTRIBUTION_HPP
#define ORF_NORMALDISTRIBUTION_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>
#include <orflib/math/stats/univariatedistribution.hpp>
#include <orflib/math/stats/errorfunction.hpp>

BEGIN_NAMESPACE(orf)

/** The normal distribution */
class NormalDistribution : public UnivariateDistribution
{
public:
  /** Initializing ctor */
  NormalDistribution(double mu = 0.0, double sigma = 1.0) : mu_(mu), sig_(sigma)
  {
    ORF_ASSERT(sig_ > 0, "error: sigma must be positive");
  }

  /** Probalbility density function */
  double pdf(double x) const
  {
    return (M_1_SQRT2PI / sig_) *
           std::exp(-0.5 * std::pow((x - mu_) / sig_, 2));
  }

  /** Cumulative distribution function */
  double cdf(double x) const
  {
    return 0.5 * ErrorFunction::erfc(-M_SQRT1_2 * (x - mu_) / sig_);
  }

  /** Inverse cumulative distribution function */
  double invcdf(double p) const
  {
    ORF_ASSERT(p > 0 && p < 1, "error: prob. must be in (0,1)");
    return -M_SQRT2 * sig_ * ErrorFunction::inverfc(2.0 * p) + mu_;
  }

protected:
  double mu_, sig_;
};

END_NAMESPACE(orf)

#endif // ORF_NORMALDISTRIBUTION_HPP
