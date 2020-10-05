/**
@file  xlregister1.cpp
@brief Registration of Excel callable functions
*/

#include <xlw/xlw.h>
using namespace xlw;

namespace {

  // Register the function ORF.ERF
  XLRegistration::Arg OrfErfArgs[] = {
    { "x", "x", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfErf(
    "xlOrfErf", "ORF.ERF", "The error function.",
    "ORFLIB", OrfErfArgs, 1);

  // Register the function ORF.INVERF
  XLRegistration::Arg OrfInvErfArgs[] = {
    { "y", "y", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfInvErf(
    "xlOrfInvErf", "ORF.INVERF", "The inverse error function.",
    "ORFLIB", OrfInvErfArgs, 1);

  // Register the function ORF.NORMALCDF
  XLRegistration::Arg OrfNormalCdfArgs[] = {
    { "x", "x", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfNormalCdf(
    "xlOrfNormalCdf", "ORF.NORMALCDF", "The standard normal cumulative distribution.",
    "ORFLIB", OrfNormalCdfArgs, 1);

  // Register the function ORF.NORMALINVCDF
  XLRegistration::Arg OrfNormalInvCdfArgs[] = {
    { "p", "p", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfNormalInvCdf(
    "xlOrfNormalInvCdf", "ORF.NORMALINVCDF", "The inverse standard normal cumulative distribution.",
    "ORFLIB", OrfNormalInvCdfArgs, 1);

  // Register the function ORF.FWDPRICE
  XLRegistration::Arg OrfFwdPriceArgs[] = {
    { "Spot", "spot", "XLF_OPER" },
    { "TimeToExp", "time to expiration", "XLF_OPER" },
    { "IntRate", "risk-free interest rate (cont. cmpd.)", "XLF_OPER" },
    { "DivYield", "dividend yield (cont. cmpd.)", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfFwdPrice(
    "xlOrfFwdPrice", "ORF.FWDPRICE", "The forward price of an asset.",
    "ORFLIB", OrfFwdPriceArgs, 4);

  // Register the function ORF.QFWDPRICE
  XLRegistration::Arg OrfQFwdPriceArgs[] = {
    { "Spot", "spot", "XLF_OPER" },
    { "TimeToExp", "time to expiration", "XLF_OPER" },
    { "IntRate", "risk-free interest rate (cont. cmpd)", "XLF_OPER" },
    { "DivYield", "dividend yield (cont. cmpd)", "XLF_OPER" },
    { "AssetVol", "asset volatility", "XLF_OPER" },
    { "FxVol", "FX volatility", "XLF_OPER" },
    { "Correl", "Asset-FX correlation", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfQFwdPrice(
    "xlOrfQFwdPrice", "ORF.QFWDPRICE",
    "The quanto forward price of an asset.",
    "ORFLIB", OrfQFwdPriceArgs, 7);

  // Register the function ORF.DIGIBS
  XLRegistration::Arg OrfDigiBSArgs[] = {
    { "PayoffType", "1: call; -1: put", "XLF_OPER" },
    { "Spot", "spot", "XLF_OPER" },
    { "Strike", "strike", "XLF_OPER" },
    { "TimeToExp", "time to expiration", "XLF_OPER" },
    { "IntRate", "risk-free interest rate (cont. cmpd.)", "XLF_OPER" },
    { "DivYield", "dividend yield (cont. cmpd.)", "XLF_OPER" },
    { "Vol", "volatility", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfDigiBS(
    "xlOrfDigiBS", "ORF.DIGIBS", 
    "Price of a digital option in the Black-Scholes model.",
    "ORFLIB", OrfDigiBSArgs, 7);

  // Register the function ORF.EUROBS
  XLRegistration::Arg OrfEuroBSArgs[] = {
    { "PayoffType", "1: call; -1: put", "XLF_OPER" },
    { "Spot", "spot", "XLF_OPER" },
    { "Strike", "strike", "XLF_OPER" },
    { "TimeToExp", "time to expiration", "XLF_OPER" },
    { "IntRate", "risk-free interest rate (cont. cmpd)", "XLF_OPER" },
    { "DivYield", "dividend yield (cont. cmpd)", "XLF_OPER" },
    { "Vol", "volatility", "XLF_OPER" },
    { "Headers", "TRUE for displaying the header. Default: FALSE", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfEuroBS(
    "xlOrfEuroBS", "ORF.EUROBS",
    "Price and Greeks of a European option in the Black-Scholes model.",
    "ORFLIB", OrfEuroBSArgs, 8);

  // Register the function ORF.KOFWD
  XLRegistration::Arg OrfKOFwdArgs[] = {
      {"Spot", "spot", "XLF_OPER"},
      {"Strike", "strike", "XLF_OPER"},
      {"KOLevel", "knockout level", "XLF_OPER"},
      {"TimeToExp", "time to expiration", "XLF_OPER"},
      {"TimeToKO", "time to knockout", "XLF_OPER"},
      {"IntRate", "risk-free interest rate (cont. cmpd)", "XLF_OPER"},
      {"DivYield", "dividend yield (cont. cmpd)", "XLF_OPER"},
      {"Vol", "volatility", "XLF_OPER"}
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfKOFwd(
    "xlOrfKOFwd", "ORF.KOFWD",
    "Price of a single point knockout forward contract in the Black-Scholes model.",
    "ORFLIB", OrfKOFwdArgs, 8);

  // Register the function ORF.BARRBS
  XLRegistration::Arg OrfBarrBSArgs[] = {
	  {"PayoffType", "payoff type", "XLF_OPER"},
	  {"BarrierType", "barrier type", "XLF_OPER"},
	  {"Spot", "spot", "XLF_OPER"},
	  {"Strike", "strike", "XLF_OPER"},
	  {"Barrier", "barrier", "XLF_OPER"},
	  {"TimeToExp", "time to expiration", "XLF_OPER"},
	  {"IntRate", "risk-free interest rate (cont. cmpd)", "XLF_OPER"},
	  {"DivYield", "dividend yield (cont. cmpd)", "XLF_OPER"},
	  {"Vol", "volatility", "XLF_OPER"}
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfBarrBSwd(
	  "xlOrfBarrBS", "ORF.BARRBS",
	  "Price of a barrier option in the Black-Scholes model.",
	  "ORFLIB", OrfBarrBSArgs, 9);

}
