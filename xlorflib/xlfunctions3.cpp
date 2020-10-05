/**
@file  xlfunctions3.cpp
@brief Implementation of Excel callable functions
*/

#include <orflib/market/market.hpp>
#include <orflib/products/europeancallput.hpp>
#include <orflib/products/asianbasketcallput.hpp>
#include <orflib/pricers/bsmcpricer.hpp>
#include <orflib/pricers/multiassetbsmcpricer.hpp>
#include <orflib/math/stats/meanvarcalculator.hpp>

#include <xlorflib/xlutils.hpp>
#include <xlw/xlw.h>

#include <cmath>

using namespace xlw;
using namespace orf;

BEGIN_EXTERN_C

LPXLFOPER EXCEL_EXPORT xlOrfEuroBSMC(LPXLFOPER xlPayoffType,
                                     LPXLFOPER xlStrike,
                                     LPXLFOPER xlTimeToExp,
                                     LPXLFOPER xlSpot,
                                     LPXLFOPER xlDiscountCrv,
                                     LPXLFOPER xlDivYield,
                                     LPXLFOPER xlVolatility,
                                     LPXLFOPER xlMcParams,
                                     LPXLFOPER xlNPaths,
                                     LPXLFOPER xlHeaders)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  int payoffType = XlfOper(xlPayoffType).AsInt();
  double spot = XlfOper(xlSpot).AsDouble();
  double strike = XlfOper(xlStrike).AsDouble();
  double timeToExp = XlfOper(xlTimeToExp).AsDouble();

  std::string name = xlStripTick(XlfOper(xlDiscountCrv).AsString());
  SPtrYieldCurve spyc = market().yieldCurves().get(name);
  ORF_ASSERT(spyc, "error: yield curve " + name + " not found");

  double divYield = XlfOper(xlDivYield).AsDouble();
  double vol = XlfOper(xlVolatility).AsDouble();
  // read the MC parameters
  McParams mcparams = xlOperToMcParams(XlfOper(xlMcParams));
  // read the number of paths
  unsigned long npaths = XlfOper(xlNPaths).AsInt();
  // handling the xlHeaders argument
  bool headers;
  if (XlfOper(xlHeaders).IsMissing() || XlfOper(xlHeaders).IsNil())
    headers = false;
  else
    headers = XlfOper(xlHeaders).AsBool();

  // create the product
  SPtrProduct spprod(new EuropeanCallPut(payoffType, strike, timeToExp));
  // create the pricer
  BsMcPricer bsmcpricer(spprod, spyc, divYield, vol, spot, mcparams);
  // create the statistics calculator
  MeanVarCalculator<double *> sc(bsmcpricer.nVariables());
  // run the simulation
  bsmcpricer.simulate(sc, npaths);
  // collect results
  Matrix const& results = sc.results();
  // read out results
  size_t nsamples = sc.nSamples();
  double mean = results(0, 0);
  double stderror = results(1, 0);
  stderror = std::sqrt(stderror / nsamples);

  // write results to the outbound XlfOper
  RW offset = headers ? 1 : 0;
  XlfOper xlRet(2 + offset, 1); // construct a range of size 2 x 1
  if (headers) {
    xlRet(0, 0) = "Price";
  }
  xlRet(offset, 0) = mean;
  xlRet(offset + 1, 0) = stderror;

  return xlRet;

  EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfAsianBasketBSMC(LPXLFOPER xlPayoffType,
                                            LPXLFOPER xlStrike,
                                            LPXLFOPER xlFixingTimes,
                                            LPXLFOPER xlAssetQuantities,
                                            LPXLFOPER xlSpots,
                                            LPXLFOPER xlDiscountCrv,
                                            LPXLFOPER xlDivYields,
                                            LPXLFOPER xlVolatilities,
                                            LPXLFOPER xlCorrelationMatrix,
                                            LPXLFOPER xlMcParams,
                                            LPXLFOPER xlNPaths,
                                            LPXLFOPER xlHeaders)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  int payoffType = XlfOper(xlPayoffType).AsInt();
  double strike = XlfOper(xlStrike).AsDouble();
  Vector fixingTimes = xlOperToVector(XlfOper(xlFixingTimes));
  Vector assetQuantities = xlOperToVector(XlfOper(xlAssetQuantities));
  Vector spots = xlOperToVector(XlfOper(xlSpots));

  std::string name = xlStripTick(XlfOper(xlDiscountCrv).AsString());
  SPtrYieldCurve spyc = market().yieldCurves().get(name);
  ORF_ASSERT(spyc, "error: yield curve " + name + " not found");

  Vector divYields = xlOperToVector(XlfOper(xlDivYields));
  Vector vols = xlOperToVector(XlfOper(xlVolatilities));
  Matrix correlMat = xlOperToMatrix(XlfOper(xlCorrelationMatrix));

  // read the MC parameters
  McParams mcparams = xlOperToMcParams(XlfOper(xlMcParams));
  // read the number of paths
  unsigned long npaths = XlfOper(xlNPaths).AsInt();
  // handling the xlHeaders argument
  bool headers;
  if (XlfOper(xlHeaders).IsMissing() || XlfOper(xlHeaders).IsNil())
    headers = false;
  else
    headers = XlfOper(xlHeaders).AsBool();

  // create the product
  SPtrProduct spprod(new AsianBasketCallPut(payoffType, strike, fixingTimes, assetQuantities));
  // create the pricer
  MultiAssetBsMcPricer bsmcpricer(spprod, spyc, divYields, vols, spots, correlMat, mcparams);
  // create the statistics calculator
  MeanVarCalculator<double *> sc(bsmcpricer.nVariables());
  // run the simulation
  bsmcpricer.simulate(sc, npaths);
  // collect results
  Matrix const & results = sc.results();
  // read out results
  double mean = results(0, 0);
  double stderror = results(1, 0);
  stderror = std::sqrt(stderror / sc.nSamples());

  // write results to the outbound XlfOper
  RW offset = headers ? 1 : 0;
  XlfOper xlRet(2 + offset, 1); // construct a range of size 2 x 1
  if (headers) {
    xlRet(0, 0) = "Price";
  }
  xlRet(offset, 0) = mean;
  xlRet(offset + 1, 0) = stderror;

  return xlRet;

  EXCEL_END;
}

END_EXTERN_C
