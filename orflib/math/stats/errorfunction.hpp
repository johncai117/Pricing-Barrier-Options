/**
@file  errorfunction.hpp
@brief Error function, its complement and its inverse
*/

#ifndef ORF_ERRORFUNCTION_HPP
#define ORF_ERRORFUNCTION_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>

BEGIN_NAMESPACE(orf)

/** Class containing functions related to the error function. 
    The functions are static members, no objects of class ErrorFunction can be created.
    Implementation adapted from Numerical Recipes in C++ 3ed. 
*/
class ErrorFunction
{
public:
  /** Returns the error function erf(x) */
  static double erf(double x);

  /** Returns the complement 1-erf(x) */
  static double erfc(double x);

  /** Returns the inverse of the error function */
  static double inverf(double p);

  /** Returns the inverse of the complement of the error function */
  static double inverfc(double p);

private:
  // This class is a namespace for error function related functions.
  // No default or copy construction or assignment allowed.
  ErrorFunction() = delete;
  ErrorFunction(ErrorFunction const&) = delete;
  ErrorFunction& operator=(ErrorFunction const&) = delete;

  /** Calculates the complement of the error function using Chebyshev polynomial */
  static double erfccheb(double z);

  // state
  static const int ncof = 28;
  static const double cof[28];
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline double ErrorFunction::erf(double x)
{
  // Return erf.x/ for any x.
  if (x >= 0.)
    return 1.0 - erfccheb(x);
  else
    return erfccheb(-x) - 1.0;
}

inline double ErrorFunction::erfc(double x)
{
  // Return erfc.x/ for any x.
  if (x >= 0.)
    return erfccheb(x);
  else
    return 2.0 - erfccheb(-x);
}

inline double ErrorFunction::inverf(double p)
{
  return inverfc(1. - p);
}

END_NAMESPACE(orf)

#endif // ORF_ERRORFUNCTION_HPP
