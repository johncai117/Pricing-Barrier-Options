/**
@file  xlfunctions2.cpp
@brief Implementation of Excel callable functions
*/
#include <orflib/defines.hpp>
#include <orflib/market/market.hpp>
#include <orflib/pricers/simplepricers.hpp>

#include <xlorflib/xlutils.hpp>
#include <xlw/xlw.h>

using namespace xlw;
using namespace orf;

BEGIN_EXTERN_C

LPXLFOPER EXCEL_EXPORT xlOrfMktList()
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::vector<std::string> ycnames = market().yieldCurves().list();
  std::vector<std::string> volnames = market().volatilities().list();
  RW nrows = (RW)(1 + std::max(ycnames.size(), volnames.size()));

  XlfOper xlRet(nrows, 2);
  xlRet(0, 0) = "YieldCurves";
  xlRet(0, 1) = "Volatilities";
  for (RW i = 1; i < xlRet.rows(); ++i) {
    xlRet(i, 0) = "";
    xlRet(i, 1) = "";
  }
  for (RW i = 0; i < (RW)ycnames.size(); ++i)
    xlRet(i + 1, 0) = ycnames[i];
  for (RW i = 0; i < (RW)volnames.size(); ++i)
    xlRet(i + 1, 1) = volnames[i];

  return xlRet;
  EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfMktClear()
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  market().clear();
  XlfOper xlRet(true);

  return xlRet;
  EXCEL_END;
}


LPXLFOPER EXCEL_EXPORT xlOrfYCCreate(LPXLFOPER xlYCName,
                                     LPXLFOPER xlTMats,
                                     LPXLFOPER xlVals,
                                     LPXLFOPER xlValType)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::string name = XlfOper(xlYCName).AsString();
  Vector tmats = xlOperToVector(XlfOper(xlTMats));
  Vector vals = xlOperToVector(XlfOper(xlVals));

  int valtype = XlfOper(xlValType).AsInt();
  YieldCurve::InputType intype;
  switch (valtype) {
  case 0:
    intype = YieldCurve::InputType::SPOTRATE;
    break;
  case 1:
    intype = YieldCurve::InputType::FWDRATE;
    break;
  case 2:
    intype = YieldCurve::InputType::ZEROBOND;
    break;
  default:
    ORF_ASSERT(0, "error: unknown yield curve input type");
  }

  std::pair<std::string, unsigned long> pr =
    market().yieldCurves().set(name,
     std::make_shared<YieldCurve>(tmats.begin(), tmats.end(), vals.begin(), vals.end(), intype)
    );

  std::string tag = xlAddTick(pr.first);
  tag += std::to_string(pr.second);
  return XlfOper(tag);
  EXCEL_END;
}


LPXLFOPER EXCEL_EXPORT xlOrfDiscount(LPXLFOPER xlCrvName,
                                     LPXLFOPER xlMat)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::string name = xlStripTick(XlfOper(xlCrvName).AsString());
  double tmat = XlfOper(xlMat).AsDouble();

  std::shared_ptr<YieldCurve> spyc = market().yieldCurves().get(name);
  ORF_ASSERT(spyc, "error: yield curve " + name + " not found");

  double df = spyc->discount(tmat);
  return XlfOper(df);
  EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfFwdDiscount(LPXLFOPER xlCrvName,
                                        LPXLFOPER xlMat1,
                                        LPXLFOPER xlMat2)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::string name = xlStripTick(XlfOper(xlCrvName).AsString());
  double T1 = XlfOper(xlMat1).AsDouble();
  double T2 = XlfOper(xlMat2).AsDouble();

  std::shared_ptr<YieldCurve> spyc = market().yieldCurves().get(name);
  ORF_ASSERT(spyc, "error: yield curve " + name + " not found");

  double fdf = spyc->fwdDiscount(T1, T2);
  return XlfOper(fdf);
  EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfSpotRate(LPXLFOPER xlCrvName,
                                     LPXLFOPER xlMat)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::string name = xlStripTick(XlfOper(xlCrvName).AsString());
  double tmat = XlfOper(xlMat).AsDouble();

  std::shared_ptr<YieldCurve> spyc = market().yieldCurves().get(name);
  ORF_ASSERT(spyc, "error: yield curve " + name + " not found");

  double srate = spyc->spotRate(tmat);
  return XlfOper(srate);
  EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfFwdRate(LPXLFOPER xlCrvName,
                                    LPXLFOPER xlMat1,
                                    LPXLFOPER xlMat2)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::string name = xlStripTick(XlfOper(xlCrvName).AsString());
  double T1 = XlfOper(xlMat1).AsDouble();
  double T2 = XlfOper(xlMat2).AsDouble();

  std::shared_ptr<YieldCurve> spyc = market().yieldCurves().get(name);
  ORF_ASSERT(spyc, "error: yield curve " + name + " not found");

  double frate = spyc->fwdRate(T1, T2);
  return XlfOper(frate);
  EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfVolCreate(LPXLFOPER xlVolName,
                                      LPXLFOPER xlTMats,
                                      LPXLFOPER xlVals,
                                      LPXLFOPER xlValType)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::string name = XlfOper(xlVolName).AsString();
  Vector tmats = xlOperToVector(XlfOper(xlTMats));
  Vector vals = xlOperToVector(XlfOper(xlVals));

  int valtype = XlfOper(xlValType).AsInt();
  VolatilityTermStructure::VolType voltype;
  switch (valtype) {
  case 0:
    voltype = VolatilityTermStructure::VolType::SPOTVOL;
    break;
  case 1:
    voltype = VolatilityTermStructure::VolType::FWDVOL;
    break;
  default:
    ORF_ASSERT(0, "error: unknown volatility input type");
  }

  std::pair<std::string, unsigned long> pr =
    market().volatilities().set(name,
      std::make_shared<VolatilityTermStructure>(tmats.begin(), tmats.end(), vals.begin(), vals.end(), voltype)
    );

  std::string tag = xlAddTick(pr.first);
  tag += std::to_string(pr.second);
  return XlfOper(tag);
  EXCEL_END;
}


LPXLFOPER EXCEL_EXPORT xlOrfSpotVol(LPXLFOPER xlVolName,
                                    LPXLFOPER xlMat)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::string name = xlStripTick(XlfOper(xlVolName).AsString());
  double tmat = XlfOper(xlMat).AsDouble();

  std::shared_ptr<VolatilityTermStructure> spvol = market().volatilities().get(name);
  ORF_ASSERT(spvol, "error: volatility cruve " + name + " not found");

  double svol = spvol->spotVol(tmat);
  return XlfOper(svol);
  EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfFwdVol(LPXLFOPER xlVolName,
                                   LPXLFOPER xlMat1,
                                   LPXLFOPER xlMat2)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::string name = xlStripTick(XlfOper(xlVolName).AsString());
  double T1 = XlfOper(xlMat1).AsDouble();
  double T2 = XlfOper(xlMat2).AsDouble();

  std::shared_ptr<VolatilityTermStructure> spvol = market().volatilities().get(name);
  ORF_ASSERT(spvol, "error: volatility curve " + name + " not found");

  double fvol = spvol->fwdVol(T1, T2);
  return XlfOper(fvol);
  EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfCapFloorletBS(LPXLFOPER xlPayType,
                                          LPXLFOPER xlCrvName,
                                          LPXLFOPER xlStrikeRate,
                                          LPXLFOPER xlTimeToReset,
                                          LPXLFOPER xlTenor,
                                          LPXLFOPER xlFwdRateVol)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  int payoffType = XlfOper(xlPayType).AsInt();
  std::string ycname = xlStripTick(XlfOper(xlCrvName).AsString());
  double strikeRate = XlfOper(xlStrikeRate).AsDouble();
  double T1 = XlfOper(xlTimeToReset).AsDouble();
  double DeltaT = XlfOper(xlTenor).AsDouble();      // tenor of fwd rate
  double fwdvol = XlfOper(xlFwdRateVol).AsDouble();

  SPtrYieldCurve spyc = market().yieldCurves().get(ycname);
  ORF_ASSERT(spyc, "error: yield curve " + ycname + " not found");

  double price = capFloorletBS(payoffType, spyc, strikeRate, T1, DeltaT, fwdvol);
  return XlfOper(price);
  EXCEL_END;
}

LPXLFOPER EXCEL_EXPORT xlOrfCDSPV(LPXLFOPER xlCrvName,
                                  LPXLFOPER xlCredSpread,
                                  LPXLFOPER xlCdsRate,
                                  LPXLFOPER xlRecov,
                                  LPXLFOPER xlTimeToMat,
                                  LPXLFOPER xlPayFreq)
{
  EXCEL_BEGIN;

  if (XlfExcel::Instance().IsCalledByFuncWiz())
    return XlfOper(true);

  std::string ycname = xlStripTick(XlfOper(xlCrvName).AsString());
  double creadspread = XlfOper(xlCredSpread).AsDouble();
  double cdsrate = XlfOper(xlCdsRate).AsDouble();
  double recov = XlfOper(xlRecov).AsDouble();
  double timetomat = XlfOper(xlTimeToMat).AsDouble();
  int payfreq = XlfOper(xlPayFreq).AsInt();

  SPtrYieldCurve spyc = market().yieldCurves().get(ycname);
  ORF_ASSERT(spyc, "error: yield curve " + ycname + " not found");

  Vector cdspv = cdsPV(spyc, creadspread, cdsrate, recov, timetomat, payfreq);
  XlfOper ret = xlVectorToOper(cdspv, false);

  return ret;
  EXCEL_END;
}

END_EXTERN_C
