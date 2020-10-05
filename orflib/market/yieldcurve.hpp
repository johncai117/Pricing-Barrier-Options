/**
@file  yieldcurve.hpp
@brief Class representing a yield curve
*/

#ifndef ORF_YIELDCURVE_HPP
#define ORF_YIELDCURVE_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>
#include <orflib/math/interpol/piecewisepolynomial.hpp>
#include <orflib/sptr.hpp>
#include <string>

BEGIN_NAMESPACE(orf)

/** The yield curve */
class YieldCurve
{
public:

  /** Used to qualify the type of quantities used for building the curve */
  enum class InputType
  {
    SPOTRATE,
    FWDRATE,
    ZEROBOND
  };

  /** The swap frequency */
  enum class SwapFreq
  {
    ANNUAL,       // 1/year
    SEMIANNUAL,   // 2/year
    QUARTERLY,    // 4/year
    MONTHLY,      // 12/year
    WEEKLY        // 52/year
  };

  /** The interest rate compounding frequency */
  enum class RateCmpd
  {
    CONTINOUS,
    SIMPLE
  };

  /** Ctor from times to Maturity and corresponding continuous compounded rates */
  template<typename XITER, typename YITER>
  YieldCurve(XITER tMatBegin,
             XITER tMatEnd,
             YITER rateBegin,
             YITER rateEnd,
             InputType rtype = InputType::SPOTRATE);

  /** Returns the curve currency */
  std::string ccy() const { return ccy_; }

  /** Returns the discount factor from observation date to tMat */
  double discount(double tMat) const;

  /** Returns the forward discount factor from observation date to tMat */
  double fwdDiscount(double tMat1, double tMat2) const;

  /** Returns the spot rate at time tMat */
  double spotRate(double tMat) const;

  /** Returns the forward rate between times tMat1 and tMat2 */
  double fwdRate(double tMat1, double tMat2) const;

  /** Returns the swap rate at time tMat */
  // TODO Not implemented yet, requires frequency arg
  // double swapRate(double tMat1) const;

  /** Returns the forward swap rate times tMat1 and tMat2 */
  // TODO NOt implemented yet, requires frequency arg
  // double fwdSwapRate(double tMat1, double tMat2) const;


protected:

private:
  // helper functions
  void initFromZeroBonds();
  void initFromSpotRates();
  void initFromFwdRates();

  std::string ccy_;  // the curve's currency
  PiecewisePolynomial fwdrates_;  // the piecewise constant forward rates
};

using SPtrYieldCurve = std::shared_ptr<YieldCurve>;

////////////////////////////////////////////////////////////////////////////.//
// Inline implementations

template<typename XITER, typename YITER>
YieldCurve::YieldCurve(XITER tMatBegin,
                       XITER tMatEnd,
                       YITER rateBegin,
                       YITER rateEnd,
                       InputType intype)
: ccy_("USD"), fwdrates_(tMatBegin, tMatEnd, rateBegin, 0)
{
  std::ptrdiff_t n = tMatEnd - tMatBegin;
  ORF_ASSERT(n == rateEnd - rateBegin, "YieldCurve: different number of maturities and rates");
  auto it = std::find_if_not(tMatBegin, tMatEnd, [](double x) {return x > 0.0;});
  ORF_ASSERT(it == tMatEnd, "YieldCurve: maturities must be positive");

  switch (intype) {
  case YieldCurve::InputType::ZEROBOND:
    initFromZeroBonds();
    break;
  case YieldCurve::InputType::SPOTRATE:
    initFromSpotRates();
    break;
  case YieldCurve::InputType::FWDRATE:
    initFromFwdRates();
    break;
  default:
    ORF_ASSERT(0, "error: unknown yield curve input type");
  }
}

END_NAMESPACE(orf)

#endif // ORF_YIELDCURVE_HPP
