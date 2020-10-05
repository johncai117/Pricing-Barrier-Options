/**
@file  univariatedistribution.hpp
@brief Base class for all univariate distributions
*/

#ifndef ORF_UNIVARIATEDISTRIBUTION_HPP
#define ORF_UNIVARIATEDISTRIBUTION_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>

BEGIN_NAMESPACE(orf)

class UnivariateDistribution
{
public:
  /** Initializing ctor */
  UnivariateDistribution() {}

  /** Probability density function */
  virtual double pdf(double x) const = 0;

  /** Cumulative distribution function */
  virtual double cdf(double x) const = 0;

  /** Inverse cumulative distribution function */
  virtual double invcdf(double p) const = 0;
};

END_NAMESPACE(orf)

#endif  // ORF_UNIVARIATEDISTRIBUTION_HPP
