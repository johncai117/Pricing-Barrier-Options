/**
@file  tridiagonalops1d.hpp
@brief Definition of the TridiagonalOperator1D base class and the derived classes
       IdentityOperator1D, DeltaOperator1D, GammaOperator1D.
*/

#ifndef ORF_TRIDIAGONALOPS1D_HPP
#define ORF_TRIDIAGONALOPS1D_HPP

#include <orflib/exception.hpp>
#include <orflib/math/matrix.hpp>
#include <algorithm>

BEGIN_NAMESPACE(orf)

/** Utility function for inverting a tridiagonal matrix T, i.e. solving the system T*x=y.
    The matrix is input as three equal size vectors (lower, diag, upper) of size N.
    The right-hand side is input as the vector y of size N.
    The solution vector x is modified in place.
    CAUTION: all vectors must be of size N. This is not checked by the function.

    The first and last elements of lower, diag, upper and y are ignored.
    Only the elements x[1] ... x[N-2] are modified.
*/
template <typename ARRAY1, typename ARRAY2>
void solveTridiagonal(ARRAY2& x,
                      ARRAY1	const& lower,
                      ARRAY1	const& diag,
                      ARRAY1	const& upper,
                      ARRAY2	const& y);

/** Utility function that adjusts the explicit and implicit operators for boundary conditions.
    The adjustment implements constant first derivative in spot space at the edge nodes
    (zero second derivative in spot space)
*/
template <typename EXPOP, typename IMPOP>
void adjustOpsForBoundaryConditions(EXPOP& opExplicit,
                                    IMPOP& opImplicit,
                                    double DX)
{
  // do implicit first
  double lowAdjustmentValueImp = opImplicit.adjustForLowerBoundaryCondition(3, 0.0, DX, 0.0, 0.0);
  double highAdjustmentValueImp = opImplicit.adjustForHigherBoundaryCondition(3, 0.0, DX, 0.0, 0.0);

  double lowAdjustmentValueExp = opExplicit.adjustForLowerBoundaryCondition(3, 0.0, DX, 0.0, 0.0);
  double highAdjustmentValueExp = opExplicit.adjustForHigherBoundaryCondition(3, 0.0, DX, 0.0, 0.0);

  opExplicit.addToLowerVal(-lowAdjustmentValueImp + lowAdjustmentValueExp);
  opExplicit.addToUpperVal(-highAdjustmentValueImp + highAdjustmentValueExp);
}


/** Adjusts the solution at the edge notes */
inline
void applyBoundaryConditions(Matrix& solution)
{
  size_t n = solution.n_rows - 2;    // n is the number of interior nodes
  size_t nLayers = solution.n_cols;  // number of layers is the number of variables
  for (size_t j = 0; j < nLayers; ++j) {
    solution(0, j) = 2.0 * solution(1, j) - solution(2, j);
    solution(n + 1, j) = 2.0 * solution(n, j) - solution(n - 1, j);
  }
}

/** Base class representing a tridiagonal operator arising in discretization of
    1-dimensional PDEs.
*/
template <class ARRAY = orf::Vector>
class TridiagonalOp1D
{
public:

  /** default ctor */
  TridiagonalOp1D() : N_(0), LowerVal_(0.0), UpperVal_(0.0) {}

  /** initializing ctor from the three diagonal vectors*/
  TridiagonalOp1D(ARRAY const& lower, ARRAY const& diag, ARRAY const& upper)
  {
    init(lower, diag, upper);
  }

  /** initializing ctor from size and constant values for the three diagonal vectors */
  TridiagonalOp1D(size_t N, double lowerConst, double diagConst, double upperConst)
  {
    init(N, lowerConst, diagConst, upperConst);
  }

  /** Initializing function, to be called after lower_, diag_, upper_ are filled by derived class */
  void init()
  {
    N_ = lower_.size() - 2;
    LowerVal_ = UpperVal_ = 0.0;
  }

  /** Initializing function */
  void init(ARRAY const& lower, ARRAY const& diag, ARRAY const& upper)
  {
    lower_ = lower;
    diag_ = diag;
    upper_ = upper;
    N_ = lower_.size() - 2;
    LowerVal_ = UpperVal_ = 0.0;
  }

  /** Initializing function */
  void init(size_t N, double lowerConst, double diagConst, double upperConst)
  {
    N_ = N;
    LowerVal_ = UpperVal_ = 0.0;
    lower_.resize(N + 2);
    std::fill(lower_.begin(), lower_.end(), lowerConst);
    diag_.resize(N + 2);
    std::fill(diag_.begin(), diag_.end(), diagConst);
    upper_.resize(N + 2);
    std::fill(upper_.begin(), upper_.end(), upperConst);
  }

  /** Adds to the lower value */
  void addToLowerVal(double lowerVal) { LowerVal_ += lowerVal; }

  /** Adds to the upper value */
  void addToUpperVal(double upperVal) { UpperVal_ += upperVal; }

  // Boundary conditions

  /** Adjust for the standard (log-linear interpolation) boundary conditions */
  void adjustStandardBoundaryConditions(const double& DX)
  {
    adjustForLowerBoundaryCondition(3, 0.0, DX, 0.0, 0.0);
    adjustForHigherBoundaryCondition(3, 0.0, DX, 0.0, 0.0);
  }

  /** Lower boundary conditions */
  double adjustForLowerBoundaryCondition(int degree,
                                         double value,
                                         double DX,
                                         double diagAdjust,
                                         double upAdjust);
  /** Higher boundary conditions */
  double adjustForHigherBoundaryCondition(int degree,
                                          double value,
                                          double DX,
                                          double diagAdjust,
                                          double lowAdjust);
  // Apply

  template <typename ARRAY1, typename ARRAY2>
  void apply(ARRAY1 const& vals, ARRAY2& result) const
  {
    result[1] = LowerVal_ + diag_[1] * vals[1] + upper_[1] * vals[2];
    for (size_t i = 2; i <= N_ - 1; ++i) {
      result[i] = lower_[i] * vals[i - 1] + diag_[i] * vals[i] + upper_[i] * vals[i + 1];
    }
    result[N_] = lower_[N_] * vals[N_ - 1] + diag_[N_] * vals[N_] + UpperVal_;
  }

  template <typename ARRAY1, typename ARRAY2>
  void applyPlus(ARRAY1 const& vals, ARRAY2& result) const
  {
    result[1] += LowerVal_ + diag_[1] * vals[1] + upper_[1] * vals[2];
    for (size_t i = 2; i <= N_ - 1; ++i) {
      result[i] += lower_[i] * vals[i - 1] + diag_[i] * vals[i] + upper_[i] * vals[i + 1];
    }
    result[N_] += lower_[N_] * vals[N_ - 1] + diag_[N_] * vals[N_] + UpperVal_;
  }

  template <typename ARRAY1, typename ARRAY2>
  void applyInverse(ARRAY1 const& vals, ARRAY2& result)
  {
    solveTridiagonal(result, lower_, diag_, upper_, vals);
  }


  // Addition, subtraction and multiplication operations

  /** Adds the rhs in place */
  template<typename ARRAY1>
  TridiagonalOp1D & operator+=(TridiagonalOp1D<ARRAY1> const& rhs);

  /** Subtracts the rhs in place */
  template<typename ARRAY1>
  TridiagonalOp1D & operator-=(TridiagonalOp1D<ARRAY1> const& rhs);

  /** Multiplies this operator by the rhs scalar */
  TridiagonalOp1D & operator*=(double rhs);

  /** Adds two Triadiagonal operators */
  friend TridiagonalOp1D operator+(TridiagonalOp1D const& a,
                                   TridiagonalOp1D const& b)
  {
    ORF_ASSERT(a.N_ == b.N_, "TridiagonalOperator1D: cannot add two operators of different sizes");
    TridiagonalOp1D<ARRAY> res;

    ARRAY lo2(a.N_ + 2), di2(a.N_ + 2), up2(a.N_ + 2);
    for (size_t i = 0; i < a.lower_.size(); ++i) {
      lo2[i] = a.lower_[i] + b.lower_[i];
      di2[i] = a.diag_[i] + b.diag_[i];
      up2[i] = a.upper_[i] + b.upper_[i];
    }
    res.init(lo2, di2, up2);

    return res;
  }

  /** Subtracts two Triadiagonal operators */
  friend TridiagonalOp1D operator-(TridiagonalOp1D const& a,
                                   TridiagonalOp1D const& b)
  {
    ORF_ASSERT(a.N_ == b.N_, "TridiagonalOperator1D: cannot subtract two operators of different sizes");
    TridiagonalOp1D<ARRAY> res;

    ARRAY lo2(a.N_ + 2), di2(a.N_ + 2), up2(a.N_ + 2);
    for (size_t i = 0; i < a.lower_.size(); ++i) {
      lo2[i] = a.lower_[i] - b.lower_[i];
      di2[i] = a.diag_[i] - b.diag_[i];
      up2[i] = a.upper_[i] - b.upper_[i];
    }
    res.init(lo2, di2, up2);

    return res;
  }
  
  /** Scalar - operator multiplication */
  friend TridiagonalOp1D operator*(double coeff,
                                   TridiagonalOp1D const& rhs)
  {
    TridiagonalOp1D<ARRAY> res = rhs;
    ARRAY lo2(rhs.N_ + 2), di2(rhs.N_ + 2), up2(rhs.N_ + 2);

    for (size_t i = 0; i < rhs.lower_.size(); ++i) {
      lo2[i] = coeff * rhs.lower_[i];
      di2[i] = coeff * rhs.diag_[i];
      up2[i] = coeff * rhs.upper_[i];
    }
    res.init(lo2, di2, up2);

    return res;
  }

protected:
  size_t N_;
  ARRAY lower_, diag_, upper_; // all of them have size N_+2

private:
  double LowerVal_, UpperVal_;
};

/** The identity operator */
template <typename ARRAY = orf::Vector >
class IdentityOp1D : public TridiagonalOp1D < ARRAY >
{
public:
  IdentityOp1D(size_t N) : TridiagonalOp1D<ARRAY>(N, 0.0, 1.0, 0.0) {}
};

/** The Delta operator */
template <typename ARRAY = orf::Vector >
class DeltaOp1D : public TridiagonalOp1D < ARRAY >
{
public:
  DeltaOp1D() {};

  template <typename ARRAY2>
  DeltaOp1D(ARRAY2 const & drifts, double DT, double DX, double theta)
  {
    init(drifts, DT, DX, theta);
  }

  template <class ARRAY2>
  void init(ARRAY2 const & drifts, double DT, double DX, double theta)
  {
    size_t N = drifts.size();
    TridiagonalOp1D<ARRAY>::lower_.resize(N + 2);
    TridiagonalOp1D<ARRAY>::diag_.resize(N + 2);
    TridiagonalOp1D<ARRAY>::upper_.resize(N + 2);
    double temp, f1 = DT * theta / (2.0 * DX);
    for (size_t i = 1; i <= N; ++i) {
      temp = drifts[i - 1] * f1;
      TridiagonalOp1D<ARRAY>::lower_[i] = -temp;
      TridiagonalOp1D<ARRAY>::diag_[i] = 0.0;
      TridiagonalOp1D<ARRAY>::upper_[i] = temp;
    }
    TridiagonalOp1D<ARRAY>::init();
  }
};

/** The Gamma Operator */
template <typename ARRAY = orf::Vector >
class GammaOp1D : public TridiagonalOp1D < ARRAY >
{
public:
  GammaOp1D() {};

  template <typename ARRAY2>
  GammaOp1D(ARRAY2 const & variances, double DT, double DX, double theta)
  {
    init(variances, DT, DX, theta);
  }

  template <typename ARRAY2>
  void init(ARRAY2 const & variances, double DT, double DX, double theta)
  {
    size_t N = variances.size();
    TridiagonalOp1D<ARRAY>::lower_.resize(N + 2);
    TridiagonalOp1D<ARRAY>::diag_.resize(N + 2);
    TridiagonalOp1D<ARRAY>::upper_.resize(N + 2);

    double f1 = 0.5 * DT * theta / DX / DX;
    for (size_t i = 1; i <= N; ++i) {
      double temp = f1 * variances[i - 1];
      TridiagonalOp1D<ARRAY>::lower_[i] = TridiagonalOp1D<ARRAY>::upper_[i] = temp;
      TridiagonalOp1D<ARRAY>::diag_[i] = -2.0*temp;
    }
    TridiagonalOp1D<ARRAY>::init();
  }
};


///////////////////////////////////////////////////////////////////////////////
// Inline definitions

template <typename ARRAY1, typename ARRAY2> inline
void solveTridiagonal(ARRAY2& x,
                      ARRAY1 const& lower,
                      ARRAY1 const& diag,
                      ARRAY1 const& upper,
                      ARRAY2 const& y)
{
  ptrdiff_t i, n = diag.size() - 2;

  static Vector D(1);
  static Vector Y(1);

  if (D.size() != n + 1)
    D = Vector(n + 1);

  if (Y.size() != n + 1)
    Y = Vector(n + 1);

  D[n] = diag[n];
  Y[n] = y[n];

  for (i = n - 1; i >= 1; i--) {
    D[i] = diag[i] - upper[i] * lower[i + 1] / D[i + 1];
    Y[i] = y[i] - upper[i] * Y[i + 1] / D[i + 1];
  }

  x[1] = Y[1] / D[1];

  for (i = 2; i <= n; i++) {
    x[i] = (Y[i] - lower[i] * x[i - 1]) / D[i];
  }
}

template<typename ARRAY>
inline
double TridiagonalOp1D<ARRAY>::adjustForLowerBoundaryCondition(
                                          int degree,
                                          double value,
                                          double DX,
                                          double diagAdjust,
                                          double upAdjust)
{
  ORF_ASSERT(diag_.size() >= 4, "TridiagonalOperator1D: grid is too small!");
  switch (degree) {
  case 0:
    return value;       // we set the actual value
  case 1:
    upper_[1] += lower_[1];
    return -lower_[1] * value;
  case 2:
    diag_[1] += 2 * lower_[1];
    upper_[1] -= lower_[1];
    return lower_[1] * value;
  case 3:
    ORF_ASSERT(value == 0.0,
      "TridiagonalOperator1D: cannot do non-zero 2nd derivative boundary condition");
    diag_[1] += 2.0 / (1.0 + DX / 2.0) * lower_[1];
    upper_[1] -= (1.0 - DX / 2.0) / (1.0 + DX / 2.0) * lower_[1];
    return 0.0;
  case 4:
    ORF_ASSERT(value == 0.0,
      "TridiagonalOperator1D: cannot do non-zero 2nd derivative boundary condition");
    diag_[1] += diagAdjust * lower_[1];
    upper_[1] += upAdjust   * lower_[1];
    return 0.0;
  default:
    ORF_ASSERT(0, "TridiagonalOperator1D: invalid degree for boundary condition");
  }
}

template<typename ARRAY>
inline
double TridiagonalOp1D<ARRAY>::adjustForHigherBoundaryCondition(
                                        int degree,
                                        double value,
                                        double DX,
                                        double diagAdjust,
                                        double lowAdjust)
{
  ORF_ASSERT(diag_.size() >= 4, "TridiagonalOperator1D: grid is too small!");
  switch (degree) {
  case 0:
    return value;  // we set the actual value
  case 1:
    lower_[N_] += upper_[N_];
    return -upper_[N_] * value;
  case 2:
    diag_[N_] += 2 * upper_[N_];
    lower_[N_] -= upper_[N_];
    return upper_[N_] * value;
  case 3:
    ORF_ASSERT(value == 0.0,
      "TridiagonalOperator1D: cannot do non-zero 2nd derivative boundary condition");
    diag_[N_] += 2.0 / (1.0 - DX / 2.0) * upper_[N_];
    lower_[N_] -= (1.0 + DX / 2.0) / (1.0 - DX / 2.0) * upper_[N_];
    return 0.0;
  case 4:
    ORF_ASSERT(value == 0.0,
      "TridiagonalOperator1D: cannot do non-zero 2nd derivative boundary condition");
    diag_[N_] += diagAdjust * upper_[N_];
    lower_[N_] += lowAdjust  * upper_[N_];
    return 0.0;
  default:
    ORF_ASSERT(0, "TridiagonalOperator1D: invalid degree for boundary condition");
  }
}

template<typename ARRAY>
template<typename ARRAY1>
inline
TridiagonalOp1D<ARRAY> &
TridiagonalOp1D<ARRAY>::operator+=(TridiagonalOp1D<ARRAY1> const& rhs)
{
  ORF_ASSERT(N_ == rhs.N_, "TridiagonalOperator1D: cannot add two operators of different sizes");
  for (size_t i = 0; i < lower_.size(); ++i) {
    lower_[i] += rhs.lower_[i];
    diag_[i] += rhs.diag_[i];
    upper_[i] += rhs.upper_[i];
  }
  return *this;
}

template<typename ARRAY>
template<typename ARRAY1>
inline
TridiagonalOp1D<ARRAY> &
TridiagonalOp1D<ARRAY>::operator-=(TridiagonalOp1D<ARRAY1> const& rhs)
{
  ORF_ASSERT(N_ == rhs.N_, "Cannot subtract two operators of different sizes");
  for (size_t i = 0; i < lower_.size(); ++i) {
    lower_[i] -= rhs.lower_[i];
    diag_[i] -= rhs.diag_[i];
    upper_[i] -= rhs.upper_[i];
  }
  return *this;
}

template<typename ARRAY>
inline
TridiagonalOp1D<ARRAY> &
TridiagonalOp1D<ARRAY>::operator*=(double rhs)
{
  for (size_t i = 0; i < lower_.size(); ++i) {
    lower_[i] *= rhs;
    diag_[i] *= rhs;
    upper_[i] *= rhs;
  }
  return *this;
}

END_NAMESPACE(orf)

#endif  // #ifndef ORF_TRIDIAGONALOPS1D_HPP
