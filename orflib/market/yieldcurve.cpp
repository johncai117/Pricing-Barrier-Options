/**
@file  yieldcurve.hpp
@brief Class representing a yield curve
*/

#include <orflib/market/yieldcurve.hpp>

BEGIN_NAMESPACE(orf)

using namespace std;

void YieldCurve::initFromZeroBonds()
{
  auto cit = fwdrates_.coeff_begin(0);

  double T1 = 0.0;                  // the observation time is t = 0;
  double p1 = 1.0;                  // bond maturing at T1
  for (size_t i = 0; i < fwdrates_.size(); ++i, ++cit) {
    double T2 = fwdrates_.breakPoint(i);
    double p2 = *cit;
    ORF_ASSERT(p2 <= 1.0 && p2 > 0, "YieldCurve: zero bond prices must in (0,1]");
    double fwdrate = std::log(p1 / p2);
    ORF_ASSERT(fwdrate >= 0.0,
      "YieldCurve: negative fwd rate between T1 = " + to_string(T1) + " and T2 = " + to_string(T2));
    double dt = T2 - T1;          // calculate DeltaT for the next iteration
    fwdrate /= dt;
    fwdrates_.setBreakPoint(i, T1); // remember, the ppoly object is right-continuous 
    *cit = fwdrate;                 // overwrite the zero bond with the fwd rate
    p1 = p2;                        // remember the bond price
    T1 = T2;                        // remember the maturity
  }
}

void YieldCurve::initFromSpotRates()
{
  auto cit = fwdrates_.coeff_begin(0);

  double T1 = fwdrates_.breakPoint(0);
  double R1 = *cit;
  fwdrates_.setBreakPoint(0, 0.0); // remember, the ppoly object is right-continuous
  ++cit;
  for (size_t i = 1; i < fwdrates_.size(); ++i, ++cit) {
    double T2 = fwdrates_.breakPoint(i);
    double R2 = *cit;
    double F = R2 * T2 - R1 * T1;
    ORF_ASSERT(F >= 0.0,
      "YieldCurve: negative fwd rate between T1 = " + to_string(T1) +" and T2 = " + to_string(T2));
    F /= (T2 - T1);
    fwdrates_.setBreakPoint(i, T1);
    *cit = F;
    T1 = T2;
    R1 = R2;
  }
}

void YieldCurve::initFromFwdRates()
{
  // just validate the fwd rates
  auto cit = fwdrates_.coeff_begin(0);
  double T1 = 0.0;
  for (size_t i = 0; i < fwdrates_.size(); ++i, ++cit) {
    double T2 = fwdrates_.breakPoints()(i);
    fwdrates_.setBreakPoint(i, T1);  // remember, the ppoly object is right-continuous
    double fwdrate = *cit;
    ORF_ASSERT(fwdrate >= 0.0,
      "YieldCurve: negative fwd rate between T1 = " + to_string(T1) + " and T2 = " + to_string(T2));
    T1 = T2;
  }
}


double YieldCurve::discount(double tMat) const
{
  ORF_ASSERT(tMat >= 0.0, "YieldCurve: negative times not allowed");
  double ldf = -fwdrates_.integral(0.0, tMat);
  return exp(ldf);
}

double YieldCurve::fwdDiscount(double tMat1, double tMat2) const
{
  ORF_ASSERT(tMat1 >= 0.0, "YieldCurve: discount factors for negative times not allowed");
  ORF_ASSERT(tMat1 <= tMat2, "YieldCurve: maturities are out of order");
  double ldf = -fwdrates_.integral(tMat1, tMat2);
  return exp(ldf);
}

double YieldCurve::spotRate(double tMat) const
{
  ORF_ASSERT(tMat >= 0.0, "YieldCurve: spot rates for negative times not allowed");
  double srate = fwdrates_.integral(0.0, tMat);
  return srate / tMat;  // return the annualized rate
}

double YieldCurve::fwdRate(double tMat1, double tMat2) const
{
  ORF_ASSERT(tMat1 >= 0.0, "YieldCurve: discount factors for negative times not allowed");
  ORF_ASSERT(tMat1 <= tMat2, "YieldCurve: maturities are out of order");
  double frate = fwdrates_.integral(tMat1, tMat2);
  return frate / (tMat2 - tMat1);  // return the annualized rate
}

END_NAMESPACE(orf)
