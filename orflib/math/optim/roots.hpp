/**
@file   roots.hpp
@brief  Root finding functions
*/

#ifndef ORF_ROOTS_HPP
#define ORF_ROOTS_HPP

#include <orflib/defines.hpp>
#include <orflib/math/matrix.hpp>
#include <algorithm>

BEGIN_NAMESPACE(orf)

/**
  Given a function pointer or functor fx defined on the interval [x1, x2], subdivide the interval 
  into n equally spaced segments, and search for zero crossings of the function.
  nroot will be set to the number of bracketing pairs found.
  If nroot is positive, the arrays xb1[0..nroot - 1] and xb2[0..nroot - 1] will be filled 
  sequentially with any bracketing pairs that are found.
  On input, these vectors may have any size, including zero; they will be resized to nroot.
*/
template <typename T>
void zbrak(T& fx, double x1, double x2, int n, orf::Vector& xb1, orf::Vector& xb2, int& nroot)
{
  int nb = 20;           // initial guess of number of bracketing subintervals
  xb1.resize(nb);
  xb2.resize(nb);
  nroot = 0;
  double dx = (x2 - x1) / n;      // determine the size of subintervals
  double x = x1;
  double fp = fx(x1);
  for (int i = 0; i < n; ++i) {
    // loop over all intervals
    double fc = fx(x += dx);
    if (fc * fp <= 0.0) {
      // if a sign change occurs, then record values for the bounds
      xb1[nroot] = x - dx;
      xb2[nroot++] = x;
      if (nroot == nb) {
        Vector tempvec1(xb1), tempvec2(xb2);
        xb1.resize(2 * nb);
        xb2.resize(2 * nb);
        for (int j = 0; j < nb; ++j) {
          xb1[j] = tempvec1[j];
          xb2[j] = tempvec2[j];
        }
        nb *= 2;
      }
    }
    fp = fc;
  }
}

/**
  Using the secant method, return the root of a function or functor func thought to lie between
  x1 and x2. The root is refined until its accuracy is xacc.
*/
template <typename T>
double rtsec(T& func, double x1, double x2, double xacc)
{
  const int MAXIT = 30;  // maximum allowed number of iterations
  double xl, rts;
  double fl = func(x1);
  double f = func(x2);
  if (std::abs(fl) < std::abs(f)) {
  // pick the bound with the smaller function value as
    rts = x1;  // the most recent guess.
    xl = x2;
    std::swap(fl, f);
  }
  else {
    xl = x1;
    rts = x2;
  }
  for (int j = 0; j < MAXIT; ++j) {
  // secant loop.
    double dx = (xl - rts) * f / (f - fl);  // increment with respect to latest value.
    xl = rts;
    fl = f;
    rts += dx;
    f = func(rts);
    if (std::abs(dx) < xacc || f == 0.0) return rts;  // convergence test
  }
  ORF_ASSERT(0, "Maximum number of iterations exceeded in rtsec");
}

END_NAMESPACE(orf)

#endif // ORF_ROOTS_HPP
