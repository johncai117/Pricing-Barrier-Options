/**
@file  piecewisepolynomial.cpp
@brief Implementation of the PiecewisePolynomial class
*/

#include <orflib/math/interpol/piecewisepolynomial.hpp>

#include <cmath>

BEGIN_NAMESPACE(orf)

double PiecewisePolynomial::operator()(double x) const
{
  size_t n(size());
  double val;

  if (x < x_(0))
    val = c_(0, 0);      // extrapolate flat to the left
  else if (x_(n - 1) <= x)
    val = c_(0, n - 1);     // extrapolate flat to the right
  else {
    size_t idx = index(x);
    val = derivative(idx, x - x_(idx), 0);
  }
  return val;
}

double PiecewisePolynomial::eval(double x, size_t k) const
{
  size_t n(size());
  double val;

  if (x < x_(0))			// the point x is to the left x_[0]
    val = (k == 0) ? c_(0, 0) : 0.0;   // flat extrapolation
  else if (x_(n - 1) <= x)
    val = (k == 0) ? c_(0, n - 1) : 0.0;   // flat extrapolation
  else {
    size_t idx = index(x);
    val = derivative(idx, x - x_(idx), k);
  }

  return val;
}

double PiecewisePolynomial::integral(double a, double b) const
{
  int isign(1);     // the sign of the integral
  if (a == b)
    return 0.0;
  else if (a > b) { // swap them around
    double tmp = a;
    a = b;
    b = tmp;
    isign = -1;
  }
  // now a <= b; find the indices of the breakpoints to the left of a and b
  ptrdiff_t idxBkpt0 = index(a);
  ptrdiff_t idxBkpt1 = index(b);
  if (idxBkpt0 == idxBkpt1) {
    // both a and b are between two breakpoints or outside the bkpt range
    size_t i = idxBkpt0 < 0 ? 0 : idxBkpt0; // if a and b are both to the left of the first bkpt
    return isign * c_(0, i) * (b - a); // constant times (b-a)
  }

  double val(0.0);  // the value of the integral
  // if a is to the left of the first bkpt, add the integral from a to x_(0)
  // use flat extrapolation
  if (idxBkpt0 == -1) {
    ++idxBkpt0;
    val += c_(0, idxBkpt0) * (x_(idxBkpt0) - a);
  }

  // first compute the stub piece between a and the first breakpoint to the right of a
  if (a > x_(idxBkpt0)) {
    val = primitive(idxBkpt0, x_(idxBkpt0 + 1) - x_(idxBkpt0), 1) - primitive(idxBkpt0, a - x_(idxBkpt0), 1);
    ++idxBkpt0;
  }
  // iterate over the bkpts in the range [x_(++idxBkpt0), x_(idxBkpt1) ) and accumulate the sum
  for (ptrdiff_t idx = idxBkpt0; idx < idxBkpt1; ++idx) {
    double xlo = x_(idx);      // the left end of the integration
    double xhi = x_(idx + 1);  // the right end of the integration
    val += primitive(idx, xhi - xlo, 1);
  }
  // finally add the stub piece between x_(idxBkpt1) and b
  val += primitive(idxBkpt1, b - x_(idxBkpt1), 1);  // the stub piece from a to the first bkpt
  return isign * val;
}


PiecewisePolynomial PiecewisePolynomial::operator+(PiecewisePolynomial const& p) const
{
  size_t n = size() + p.size();               // the sum has at most n breakpoints
  size_t ord = std::max(order(), p.order());  // the sum has order the max of the two orders
  Vector bkpts(n);                            // the breakpoints of the sum
  // merge the breakpoints
  Vector::const_iterator bkend = std::set_union(x_.begin(), x_.end(),
    p.breakPoints().begin(), p.breakPoints().end(), bkpts.begin());
  Vector::const_iterator bkbeg = bkpts.begin();

  PiecewisePolynomial psum(bkbeg, bkend, ord);
  size_t nbks = psum.size();        // number of unique breakpoints
  Vector tval(nbks), pval(nbks);

  // polynomial summation; compute derivatives of all orders and add them
  for (size_t i = 0; i <= ord; ++i) {
    eval(bkbeg, bkend, tval.begin(), i);
    p.eval(bkbeg, bkend, pval.begin(), i);
    size_t fct = factorial(i);
    Matrix::row_iterator rit = psum.coeff_begin(i);
    for (size_t j = 0; j < nbks; ++j) {
      *rit += (tval(j) + pval(j)) / fct;
      ++rit;
    }
  }
  return psum;
}

PiecewisePolynomial PiecewisePolynomial::operator*(PiecewisePolynomial const& p) const
{
  size_t n = size() + p.size();               // the product has at most n breakpoints
  size_t ord = order() + p.order();           // the product has order the sum of the orders
  Vector bkpts(n);                            // the breakpoints of the product
  // merge the breakpoints
  Vector::const_iterator bkend = std::set_union(x_.begin(), x_.end(),
    p.breakPoints().begin(), p.breakPoints().end(), bkpts.begin());
  Vector::const_iterator bkbeg = bkpts.begin();

  PiecewisePolynomial pprod(bkbeg, bkend, ord);
  size_t nbks = pprod.size();        // number of unique breakpoints
  Vector tval(nbks), pval(nbks);

  // polynomial multiplication: using convolution of coefficients
  for (size_t i = 0; i <= ord; ++i) {
    Matrix::row_iterator rit = pprod.coeff_begin(i);
    for (size_t k = 0; k <= i; ++k) {
      eval(bkbeg, bkend, tval.begin(), k);
      p.eval(bkbeg, bkend, pval.begin(), i - k);
      size_t fact1 = factorial(i);
      size_t fact2 = factorial(i - k);
      for (size_t j = 0; j < nbks; ++j) {
        double tmp = tval[j] / fact1;
        tmp *= pval[j] / fact2;
        *rit += tmp;
        ++rit;
      }
    }
  }
  return pprod;
}

END_NAMESPACE(orf)
