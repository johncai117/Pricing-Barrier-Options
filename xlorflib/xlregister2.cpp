/** 
@file  xlregister2.cpp
@brief Registration of Excel callable functions
*/

#include <xlw/xlw.h>
using namespace xlw;

namespace {

  // Register the function ORF.MKTLIST
  XLRegistration::Arg OrfMktListArgs[] = {
    {}
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfMktList(
    "xlOrfMktList", "ORF.MKTLIST", "Lists all market objects",
    "ORFLIB", OrfMktListArgs, 0);

  // Register the function ORF.MKTCLEAR
  XLRegistration::Arg OrfMktClearArgs[] = {
    {}
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfMktClear(
    "xlOrfMktClear", "ORF.MKTCLEAR", "Deletes all market objects",
    "ORFLIB", OrfMktClearArgs, 0);

  // Register the function ORF.YCCREATE
  XLRegistration::Arg OrfYCCreateArgs[] = {
    { "YCName", "Yield curve name", "XLF_OPER" },
    { "Mats", "Time to each maturity", "XLF_OPER" },
    { "Rates", "Interest rates or zero coupon bond prices", "XLF_OPER" },
    { "InputType", "0: spot rates, 1: fwd rates, 2: zero coupon bonds", "XLF_OPER" },
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfYCCreate(
    "xlOrfYCCreate", "ORF.YCCREATE", "Creates a yield curve",
    "ORFLIB", OrfYCCreateArgs, 4);

  // Register the function ORF.DF
  XLRegistration::Arg OrfDiscountArgs[] = {
    { "YCName", "Yield curve name", "XLF_OPER" },
    { "Mat", "Time to maturity", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfDiscount(
    "xlOrfDiscount", "ORF.DF", "Returns the discount factor to maturity",
    "ORFLIB", OrfDiscountArgs, 2);

  // Register the function ORF.FDF
  XLRegistration::Arg OrfFwdDiscountArgs[] = {
    { "YCName", "Yield curve name", "XLF_OPER" },
    { "Mat1", "Time to first maturity", "XLF_OPER" },
    { "Mat2", "Time to second maturity", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfFwdDiscount(
    "xlOrfFwdDiscount", "ORF.FWDDF", "Returns the fwd discount factor between the two maturities",
    "ORFLIB", OrfFwdDiscountArgs, 3);

  // Register the function ORF.SPOTRATE
  XLRegistration::Arg OrfSpotRateArgs[] = {
    { "YCName", "Yield curve name", "XLF_OPER" },
    { "Mat", "Time to maturity", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfSpotRate(
    "xlOrfSpotRate", "ORF.SPOTRATE", "Returns the spot rate to maturity",
    "ORFLIB", OrfSpotRateArgs, 2);

  // Register the function ORF.FWDRATE
  XLRegistration::Arg OrfFwdRateArgs[] = {
    { "YCName", "Yield curve name", "XLF_OPER" },
    { "Mat1", "Time to first maturity", "XLF_OPER" },
    { "Mat2", "Time to second maturity", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfFwdRate(
    "xlOrfFwdRate", "ORF.FWDRATE", "Returns the fwd rate between the two maturities",
    "ORFLIB", OrfFwdRateArgs, 3);

  // Register the function ORF.VOLCREATE
  XLRegistration::Arg OrfVolCreateArgs[] = {
    { "VCName", "Volatility curve name", "XLF_OPER" },
    { "MatRange", "Time to each maturity", "XLF_OPER" },
    { "VolRange", "Market volatilities", "XLF_OPER" },
    { "VolType", "0: spot volatility, 1: fwd volatility", "XLF_OPER" },
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfVolCreate(
    "xlOrfVolCreate", "ORF.VOLCREATE", "Creates a volatility curve",
    "ORFLIB", OrfVolCreateArgs, 4);

  // Register the function ORF.SPOTVOL
  XLRegistration::Arg OrfSpotVolArgs[] = {
    { "VCName", "Volatility curve name", "XLF_OPER" },
    { "Maturity", "Time to maturity", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfSpotVol(
    "xlOrfSpotVol", "ORF.SPOTVOL", "Returns the spot volatility to maturity",
    "ORFLIB", OrfSpotVolArgs, 2);

  // Register the function ORF.FWDVOL
  XLRegistration::Arg OrfFwdVolArgs[] = {
    { "VCName", "Volatility curve name", "XLF_OPER" },
    { "Mat1", "Time to first maturity", "XLF_OPER" },
    { "Mat2", "Time to second maturity", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfFwdVol(
    "xlOrfFwdVol", "ORF.FWDVOL", "Returns the fwd volatility between the two maturities",
    "ORFLIB", OrfFwdVolArgs, 3);

  // Register the function ORF.CAPFLOORLETBS
  XLRegistration::Arg OrfCapBloorletBSArgs[] = {
    { "PayType", "payoff type; caplet:1, floorlet:-1", "XLF_OPER" },
    { "YCName", "Yield curve name", "XLF_OPER" },
    { "StrikeRate", "Strike forward rate with simple compounding", "XLF_OPER" },
    { "TimeToReset", "Time to reset in years", "XLF_OPER" },
    { "Tenor", "The tenor of the forward rate in years", "XLF_OPER" },
    { "FwdRateVol", "The annualized volatility of the forward rate", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfCapFloorletBS(
    "xlOrfCapFloorletBS", "ORF.CAPFLOORLETBS", "Returns the present value of a caplet/floorlet on fwd rate",
    "ORFLIB", OrfCapBloorletBSArgs, 6);

  // Register the function ORF.CDSPV
  XLRegistration::Arg OrfCDSPVArgs[] = {
    { "RFreeYC", "Risk-free yield curve name", "XLF_OPER" },
    { "CreadSpread", "Credit spread with simple compounding", "XLF_OPER" },
    { "CDSRate", "CDS rate with simple compounding", "XLF_OPER" },
    { "Recov", "Recovery rate in case of default", "XLF_OPER" },
    { "TimeToMat", "Time to maturity in years", "XLF_OPER" },
    { "PayFreq", "Annual payment frequency", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfCDSPV(
    "xlOrfCDSPV", "ORF.CDSPV", "Returns the present value of a CDS",
    "ORFLIB", OrfCDSPVArgs, 6);  

}  // anonymous namespace
