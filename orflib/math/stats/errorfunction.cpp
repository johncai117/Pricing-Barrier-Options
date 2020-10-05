/**
@file  errorfunction.cpp
@brief Implementation of functions related to the mathematical error function
*/

#include <orflib/math/stats/errorfunction.hpp>
#include <cmath>
using namespace std;

BEGIN_NAMESPACE(orf)

/* From Numerical Recipes in C++ 3 ed. 
Chebyshev polynomial used in the calculation of the complement of the error function
*/
const double ErrorFunction::cof[28] = {
  -1.3026537197817094,
  6.4196979235649026e-1,
  1.9476473204185836e-2,
  -9.561514786808631e-3,
  -9.46595344482036e-4,
  3.66839497852761e-4,
  4.2523324806907e-5,
  -2.0278578112534e-5,
  -1.624290004647e-6,
  1.303655835580e-6,
  1.5626441722e-8,
  -8.5238095915e-8,
  6.529054439e-9,
  5.059343495e-9,
  -9.91364156e-10,
  -2.27365122e-10,
  9.6467911e-11,
  2.394038e-12,
  -6.886027e-12,
  8.94487e-13,
  3.13092e-13,
  -1.12708e-13,
  3.81e-16,
  7.106e-15,
  -1.523e-15,
  -9.4e-17,
  1.21e-16,
  -2.8e-17
};

double ErrorFunction::erfccheb(double z)
{
  // Complementary error function. 
  // Evaluates equation (6.2.16) in NUmerical Recipes 3ed, using stored Chebyshev coefficients.
  ORF_ASSERT(z >= 0.0, "erfccheb requires non-negative argument");
  double t = 2. / (2. + z);
  double ty = 4. * t - 2.;
  double d = 0., dd = 0.;
  for (int j = ncof - 1; j > 0; j--) {
    double tmp = d;
    d = ty * d - dd + cof[j];
    dd = tmp;
  }
  return t * exp(-z * z + 0.5 * (cof[0] + ty * d) - dd);
}

double ErrorFunction::inverfc(double p)
{
  // Inverse of complementary error function. 
  // Returns x such that erfc(x) = p for argument p between 0 and 2.
  if (p >= 2.0)
    return -100.;
  //Return arbitrary large pos or neg value.
  if (p <= 0.0)
    return 100.;
  double pp = (p < 1.0) ? p : 2. - p;
  double t = sqrt(-2.*log(pp / 2.));
  // Initial guess:
  double x = -0.70711 * ((2.30753 + t * 0.27061) / (1. + t * (0.99229 + t * 0.04481)) - t);
  for (int j = 0; j < 2; j++) {
    double err = erfc(x) - pp;
    x += err / (1.12837916709551257*exp(-sqrt(x)) - x*err); // Halley.
    x = x < 0 ? 0 : x;  // NOTE added to prevent NAN at p = 1 
  }
  return (p < 1.0 ? x : -x);
}

END_NAMESPACE(orf)
