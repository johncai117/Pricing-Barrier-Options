/**
@file  pdegrid.hpp
@brief Definition of the CoordinateChange and the GridAxis classes
*/

#ifndef ORF_PDEGRID_HPP
#define ORF_PDEGRID_HPP

#include <orflib/exception.hpp>
#include <orflib/math/matrix.hpp>
#include <memory>
#include <algorithm>

BEGIN_NAMESPACE(orf)

/** Abstract coordinate change interface.
*/
class CoordinateChangeBase
{
public:

  double theta;

  virtual ~CoordinateChangeBase() {}

  template <typename PARAMS>
  void init(PARAMS const & params)
  {
    theta = params.theta;
  }

  virtual double fromRealToDiffused(double S1) = 0;

  virtual double fromDiffusedToReal(double S1) = 0;

  virtual void forwardAndVariance(double& fwd, double& vol, double T) = 0;

  virtual void driftAndVariance(double realS,
                                double realF,
                                double theta,
                                double DT,
                                double realLNVol,
                                double aCoeff,
                                double DX,
                                double& drift,
                                double& variance,
                                double& FinalVol) = 0;

  /** Computes the grid bounds Xmin and Xmax */
  virtual void bounds(double S0,
                      double fwd,
                      double vol,
                      double T,
                      double nstds,
                      double& Xmin,
                      double& Xmax) = 0;
};


/** Identity coordinate change, i.e. Diffused = Real */
class NoCoordinateChange : public CoordinateChangeBase
{
public:
  virtual double fromRealToDiffused(double S)
  {
    return S;
  }

  virtual double fromDiffusedToReal(double S)
  {
    return S;
  }

  virtual void forwardAndVariance(double & fwd, double & vol, const double & T)
  {
    // nothing to do
  }

  void driftAndVariance(double realS,
                        double realF,
                        double theta,
                        double DT,
                        double realLNVol,
                        double aCoeff,
                        double DX,
                        double& drift,
                        double& variance,
                        double& finalVol)
  {
    double vol = realLNVol * realS;
    double corr = (theta * aCoeff + 1 - theta);
    drift = (realF - realS) / corr / DT;
    variance = vol * vol;
    finalVol = sqrt(variance);
  }

  virtual void bounds(double S0,
                      double F,
                      double vol,
                      double T,
                      double nstds,
                      double& Xmin,
                      double& Xmax)
  {
    Xmin = std::min(S0, F) * exp(-0.5 * vol * vol * T - nstds * vol * sqrt(T));
    Xmax = std::max(S0, F) * exp(-0.5 * vol * vol * T + nstds * vol * sqrt(T));
  }
};

/** Logarithmic coordinate change, i.e. Diffused = log(Real) */
class LogCoordinateChange : public CoordinateChangeBase
{
public:

  virtual double fromRealToDiffused(double S)
  {
    return log(S);
  }

  virtual double fromDiffusedToReal(double X)
  {
    return exp(X);
  }

  virtual void forwardAndVariance(double & fwd, double & vol, double T)
  {
    fwd = log(fwd) - 0.5 * vol * vol * T;
  }

  virtual void bounds(double X0,
    double F,
    double vol,
    double T,
    double nstds,
    double & Xmin,
    double & Xmax)
  {
    Xmin = std::min(X0, F) - nstds * vol * sqrt(T);
    Xmax = std::max(X0, F) + nstds * vol * sqrt(T);
  }

  virtual void driftAndVariance(double realS,
                                double realF,
                                double theta,
                                double DT,
                                double realLNVol,
                                double aCoeff,
                                double DX,
                                double& drift,
                                double& variance,
                                double& finalVol)
  {
    double Xi = fromRealToDiffused(realS);
    double Deltaip1 = (fromDiffusedToReal(Xi + DX) - fromDiffusedToReal(Xi - DX)) / (2.0 * DX);
    double Gammaip1 = (fromDiffusedToReal(Xi + DX) - 2 * fromDiffusedToReal(Xi) + fromDiffusedToReal(Xi - DX));
    Gammaip1 /= (DX * DX);
    double corr = (theta*aCoeff + 1 - theta);
    drift = (realF - realS) / corr / DT / Deltaip1 - 0.5 * realLNVol * realLNVol * Gammaip1 / Deltaip1;
    variance = realLNVol * realLNVol;
    finalVol = realLNVol;
  }
};


/** Describes the discretization of a grid coordinate axis
*/
class GridAxis
{
public:
  double Xmin, Xmax, DX;    // max, min and distance between nodes
  size_t NX;                // number of interior nodes
  Vector Xlevels, Slevels;
  Vector drifts, variances, vols;
  std::shared_ptr<CoordinateChangeBase> coordinateChange;  // the coordinate change rules for this axis

  /** Default ctor uses logarithmic coordinate changes */
  GridAxis()
    : coordinateChange(new LogCoordinateChange())
  {}

  /** Sets the coordinate changes */
  void setCoordinateChange(std::shared_ptr<CoordinateChangeBase> c)
  {
    coordinateChange = c;
  }
};

END_NAMESPACE(orf)

#endif  // #ifndef ORF_PDEGRID_HPP
