/**
@file  volatilitytermstructure.hpp
@brief Implementation of the volatility term structure class.
*/

#include <orflib/market/volatilitytermstructure.hpp>

BEGIN_NAMESPACE(orf)

using namespace std;


void VolatilityTermStructure::initFromSpotVols()
{
  auto cit = fwdvars_.coeff_begin(0);

  double T1 = fwdvars_.breakPoint(0);  // the first maturity
  double V1 = *cit;                    // the first volatility
  double V1square = V1 * V1;
  *cit = V1square;                    // write back the first variance
  // remember, the ppoly object is right continuous
  // the first time break point must be zero, i.e. the first variance is effective from time zero to T1
  fwdvars_.setBreakPoint(0, 0.0);     
  ++cit;
  for (size_t i = 1; i < fwdvars_.size(); ++i, ++cit) {
    double T2 = fwdvars_.breakPoint(i);
    double V2 = *cit;
    double V2square = V2 * V2;
    double fwdvar = V2square * T2 - V1square * T1;
    ORF_ASSERT(fwdvar >= 0.0,
      "VolatilityTermStructure: negative variance between T1 = " + to_string(T1) + " and T2 = " + to_string(T2));
    fwdvar /= (T2 - T1);
    fwdvars_.setBreakPoint(i, T1);  // store the T1 not the T2 breakpoint (right continuous)
    *cit = fwdvar;                  // store the variance between T1 and T2
    T1 = T2;                        // remember the T2 breakpoint for the next iteration
    V1square = V2square;            // remember the variance up to T2 for the next iteration
  }
}

void VolatilityTermStructure::initFromFwdVols()
{
  // just validate the fwd vols
  auto cit = fwdvars_.coeff_begin(0);
  double T1 = 0.0;
  for (size_t i = 0; i < fwdvars_.size(); ++i, ++cit) {
    double T2 = fwdvars_.breakPoints()(i);
    fwdvars_.setBreakPoint(i, T1);  // store the T1 not the T2 breakpoint (remember ppoly is right continuous)
    double fwdvol = *cit;
    ORF_ASSERT(fwdvol >= 0.0,
      "VolatilityTermStructure: negative volatility between T1 = " + to_string(T1)+" and T2 = " + to_string(T2));
    *cit = fwdvol * fwdvol;        // store the forward variance between T1 and T2
    T1 = T2;                       // remember the T2 breakpoint for the next iteration
  }
}


double VolatilityTermStructure::spotVol(double tMat) const
{
  ORF_ASSERT(tMat >= 0.0, "spot volatilities for negative times not allowed");
  if (tMat == 0.0)
    tMat = 1.0e-16; // handle division by zero
  double svar = fwdvars_.integral(0.0, tMat);
  return std::sqrt(svar / tMat);  // return the annualized volatility
}

double VolatilityTermStructure::fwdVol(double tMat1, double tMat2) const
{
  ORF_ASSERT(tMat1 >= 0.0, "forward volatilities for negative times not allowed");
  ORF_ASSERT(tMat1 <= tMat2, "maturities are out of order");
  if (tMat1 == tMat2)
    tMat2 += 1.0e-16; // handle division by zero
  double fvar = fwdvars_.integral(tMat1, tMat2);
  return std::sqrt(fvar / (tMat2 - tMat1));  // return the annualized volatility
}

END_NAMESPACE(orf)
