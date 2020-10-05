/**
@file  piecewisepolynomial.hpp
@brief Class representing a piecewise polynomial curve
*/

#ifndef ORF_PIECEWISEPOLYNOMIAL_HPP
#define ORF_PIECEWISEPOLYNOMIAL_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>
#include <orflib/math/matrix.hpp>

#include <cmath>
#include <algorithm>
#include <functional>

BEGIN_NAMESPACE(orf)

/** The piecewise polynomial curve.
  This is a function f(x) defined by a sequence of breakpoints, x_i and corresponding polynomials p_i(x).
  The polynomial p_i(x) defines f(x) in the interval [x_i, x_{i+1}).
  Therefore, the curve is right continuous, i.e. f(x_i) = lim p_i(x_i + \epsilon), with positive \epsilon -> 0.
  Outside the range of the breakpoints the curve is extrapolated flat.
  The order of the curve is the highest polynomial order of its components.
*/
class PiecewisePolynomial
{
public:
  /** Default ctor */
  PiecewisePolynomial() {}

  /** Ctor from breakpoints; order = 0: constant, order = 1: linear, ... 
    All polynomial coefficients are set to zero.
  */
  template<typename ITER>
  PiecewisePolynomial(ITER xFirst, ITER xLast, size_t order);

  /** Special ctor from breakpoints and values;
      order = 0 piecewise constant, order = 1 linear continuous */
  template<typename XITER, typename YITER>
  PiecewisePolynomial(XITER xFirst, XITER xLast, YITER yFirst, size_t order);

  // Dtor
  virtual ~PiecewisePolynomial() {}

  // Properties

  /** number of breakpoints */
  size_t size() const { return x_.size(); }
  /** order of polynomial pieces */
  size_t order() const { return c_.n_rows - 1; }

  // Access

  /** Read access by index i */
  double breakPoint(size_t i) const { return x_(i); }
  /** Write access by index i */
  void setBreakPoint(size_t i, double val) { x_(i) = val; }

  /** Read-only access to breakpoints */
  Vector const& breakPoints() const { return x_; }
  /** Read-write access  to breakpoints */
  template<typename ITER>
  void setBreakPoints(ITER xFirst, ITER xLast, size_t order);

  /** Read access by indices i, j; i is the breakpoint index */
  double coefficient(size_t i, size_t j) const { return c_(i, j); }
  /** Write access by indices i, j; i is the breakpoint index */
  void setCoefficient(size_t i, size_t j, double val) { c_(i, j) = val; }

  /** Read-only access to coefficients */
  Matrix const& coefficients() const { return c_; }
  /** Read-write access to coefficients at row i, i.e. all coefficients for the ith breakpoint interval  */
  Matrix::row_iterator coeff_begin(size_t i) { return c_.begin_row(i); };
  /** Read-write access to coefficients at row i, i.e. all coefficients for the ith breakpoint interval  */
  Matrix::row_iterator coeff_end(size_t i) { return c_.end_row(i); };

  // Evaluation

  /** Evaluate y(x) */
  double operator()(double x) const;

  /** Value or derivative at one point x
    k = 0 : y(x)
    k > 0 : k-th left derivative at x
  */
  double eval(double x, size_t k = 0) const;

  /** Evaluate at each x in [xFirst, xLast)
        The results will be written by advancing yFirst;
        It assumes that [yFirst, yFirst + (xLast - xFirst)) is a valid range.
        k = 0 : y(x)
        k > 0 : k-th left derivative at x
  */
  template<typename XITER, typename YITER>
  void eval(XITER const xFirst, XITER const xLast, YITER yFirst, size_t k = 0) const;

  /** Integrate between a and b */
  double integral(double a, double b) const;

  /** Integrate from xStart to each x in [xFirst, xLast)
    If stepwise = true then integration
    The results will be written by advancing yFirst;
    It assumes that [yFirst, yFirst + (xLast - xFirst)) is a valid range.
  */
  template<typename XITER, typename YITER>
  void integral(double xStart, XITER xFirst, XITER xLast, YITER yFirst, bool stepwise = false) const;

  // Computed assignments

  /** Add a constant value to this */
  PiecewisePolynomial& operator+=(double a) { c_.row(0) += a; return *this; }
  /** Subtract a constant value from this */
  PiecewisePolynomial& operator-=(double a) { c_.row(0) -= a; return *this; }
  /** Multiply this with a constant value */
  PiecewisePolynomial& operator*=(double a) { c_ *= a; return *this; }
  /** Divide this by a constant value */
  PiecewisePolynomial& operator/=(double a) { c_ /= a; return *this; }

  // Polynomial algebra

  /** Add p to this */
  PiecewisePolynomial operator+(PiecewisePolynomial const& p) const;
  /** Multiply p with this */
  PiecewisePolynomial operator*(PiecewisePolynomial const& p) const;

  // Calculus

  /** Differentiate this k times */
  void differentiate(size_t k);

  /** Integrate this k times */
  void integrate(size_t k);

protected:

  // Throws an exception if the breakpoints are not in increasing order
  inline void assertBreakpointOrder()
  {
    Vector::const_iterator it(std::adjacent_find(x_.begin(), x_.end(), std::greater_equal<double>()));
    ORF_ASSERT(it == x_.end(), "PiecewisePolynomial: breakpoints must be in strict increasing order");
  }

  // Returns the greatest index in the vector x_ such that x_[idx] <= x;
  // It returns -1 if x < x[0] 
  inline ptrdiff_t index(double x) const
  {
    return std::upper_bound(x_.begin(), x_.end(), x) - x_.begin() - 1;
  }

  // Helper function for computing factorials
  inline size_t factorial(size_t n) const {
    return n == 0 ? 1 : n * factorial(n - 1);
  }

  // Helper function for computing derivatives. It returns k-th derivative of p at x_[xIdx] + h
  // Assumes that k <= order()
  double derivative(size_t xIdx, double h, size_t k) const;

  // Helper function for computing primitives. It returns the integral of p at x_[xIdx] + h
  double primitive(size_t xIdx, double h, size_t k) const;

  // state
  Vector x_;  // breakpoints
  Matrix c_;  // polynomial coefficients

};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions
///////////////////////////////////////////////////////////////////////////////

inline double PiecewisePolynomial::derivative(size_t xIdx, double h, size_t k) const
{
  double val(0.0);
  size_t factnp1 = factorial(order() + 1);
  for (ptrdiff_t i = order() - k; i >= 0; --i) {
    factnp1 /= (i + k + 1);
    val = c_(i + k, xIdx) * factnp1 + val * h / (i + 1);
  }
  return val;
}

inline double PiecewisePolynomial::primitive(size_t xIdx, double h, size_t k) const
{
  double val(0.0);
  size_t ord = order();
  ptrdiff_t ik = k;  // convert to integer for signed arithmetic

  if ((xIdx == 0 && h < 0) || (xIdx == size() - 1 && h > 0)) {
    // the integration range is outside the breakpoint domain
    // then compute c*h^k/k! (flat extrapolation)
    double c = c_(0, xIdx);
    double fact = 1.0;
    for (long i = 2; i <= ik; ++i)  // compute k!
      fact *= i;
    val = c * std::pow(h, ik) / fact;
  }
  else {
    //  the integration range is inside the breakpoint domain
    ptrdiff_t i;
    size_t factnp1 = factorial(order() + 1);
    for (i = ord + ik; i >= ik; --i) {
      factnp1 /= (i + k + 1);
      val = c_(i - ik, xIdx) + val * h / (i + 1);
    }
    for (; i >= 0; --i)
      val = val * h / (i + 1);
  }
  return val;
}


template<typename ITER>
PiecewisePolynomial::PiecewisePolynomial(ITER xFirst, ITER xLast, size_t order)
  : x_(xLast - xFirst), c_(++order, xLast - xFirst, arma::fill::zeros)
{
  std::copy(xFirst, xLast, x_.begin());
  assertBreakpointOrder();
}

template<typename XITER, typename YITER>
PiecewisePolynomial::PiecewisePolynomial(XITER xFirst, XITER xLast, YITER yFirst, size_t order)
  : x_(xLast - xFirst), c_(order + 1, xLast - xFirst, arma::fill::zeros)
{
  ORF_ASSERT(order < 2, "PiecewisePolynomial: only 0th and 1st order polynomials can be constructed from values");
  std::copy(xFirst, xLast, x_.begin());
  assertBreakpointOrder();

  size_t n = xLast - xFirst;
  for (size_t j = 0; j < n; ++j)
    c_(0, j) = *(yFirst + j);

  // std::copy(yFirst, yFirst + n, c_.begin_row(0)); NOTE: the iterator implementation is broken in arma

  // set first order coefficients (slopes)
  if (this->order() == 1) {
    for (size_t j = 0; j < n; ++j, xFirst++, yFirst++) {
      if (j < n - 1)
        c_(1, j) = (*(yFirst + 1) - *yFirst) / (*(xFirst + 1) - *xFirst);
      else
        c_(1, j) = c_(1, j - 1);
    }
  }
}

template<typename ITER>
inline void PiecewisePolynomial::setBreakPoints(ITER xFirst, ITER xLast, size_t order)
{
  x_.resize(xLast - xFirst);
  std::copy(xFirst, xLast, x_.begin());
}


template<typename XITER, typename YITER>
inline void PiecewisePolynomial::eval(XITER xFirst,
  XITER xLast,
  YITER yFirst,
  size_t k) const
{
  if (xFirst == xLast)
    return;		// nothing to do

  XITER xit = xFirst;
  YITER yit = yFirst;
  for (; xit < xLast; ++xit, ++yit)
    *yit = eval(*xit, k);

}

template<typename XITER, typename YITER>
inline void PiecewisePolynomial::integral(double xStart,
  XITER xFirst,
  XITER xLast,
  YITER yFirst,
  bool stepwise) const
{
  // This is not an optimal implementation
  // We iterate over common bkpts for each integration limit x. 
  // TODO: A better implementation is to iterate first over breakpoints.
  if (xFirst == xLast)
    return;		// nothing to do

  XITER xit = xFirst;
  YITER yit = yFirst;
  for (; xit < xLast; ++xit, ++yit)
    *yit = integral(xStart, *xit);
  // if stepwise is true, take adjacent differences
  if (stepwise) {
    --yit; --xit;
    for (; xit != xFirst; --xit, --yit)
      *yit -= *(yit - 1);
  }
  return;
}

END_NAMESPACE(orf)

#endif // ORF_PIECEWISEPOLYNOMIAL_HPP
