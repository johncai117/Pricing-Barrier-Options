/**
@file  interpolation1d.hpp
@brief Classes for interpolating in one dimension
*/

#ifndef ORF_INTERPOLATION1D_HPP
#define ORF_INTERPOLATION1D_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>
#include <orflib/math/matrix.hpp>

#include <cmath>
#include <algorithm>
#include <functional>

BEGIN_NAMESPACE(orf)

/** Helper function for finding the bracketing indices of a value in an ordered vector */
template <typename ARRAY>
void findIndices(ARRAY const& v, double y, size_t& i1, size_t& i2)
{
  if (y <= v[1]) {
    i1 = 0; i2 = 1;
    return;
  }
  if (y >= v[v.size() - 2]) {
    i1 = v.size() - 2; i2 = v.size() - 1;
    return;
  }

  for (size_t i = 1; i < v.size(); ++i) {
    if (y == v[i]) {
      i1 = i2 = i;
      return;
    }
    if (y < v[i]) {
      i1 = i - 1;
      i2 = i;
      return;
    }
  }
}

/** The linear interpolator class */
template <typename ARRAY>
class LinearInterpolation1D
{
public:
  /** Initializing ctor */
  LinearInterpolation1D(Vector const& xvals, ARRAY const& yvals)
    : xvals_(xvals), yvals_(yvals)
  {
    ORF_ASSERT(xvals.size() == yvals.size(), "LinearInterpolation1D: unequal vector sizes!");
  }

  int size() const
  {
    return yvals_.size();
  }

  Vector const& xValues() const
  {
    return xvals_;
  }

  ARRAY const& yValues() const
  {
    return yvals_;
  }

  double getValue(size_t i) const
  {
    return yvals_[i];
  }

  /** Returns the y values by linearly interpolating between neighboring values
  */
  double getValue(double x) const
  {
    size_t i1, i2;
    findIndices(xvals_, x, i1, i2);
    double y1, y2;
    y1 = getValue(i1);
    if (i2 == i1) {
      return y1;
    }
    y2 = getValue(i2);
    double x1 = xvals_[i1], x2 = xvals_[i2];
    return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
  }

protected:

  Vector const& xvals_;
  ARRAY  const& yvals_;

};


END_NAMESPACE(orf)

#endif // ORF_INTERPOLATION1D_HPP
