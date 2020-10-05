/** 
@file  xlregister5.cpp
@brief Registration of Excel callable functions
*/

#include <xlw/xlw.h>
using namespace xlw;

namespace {

  // Register the function ORF.PTRISK
  XLRegistration::Arg OrfPtRiskArgs[] = {
    { "PtWghts", "portfolio weights", "XLF_OPER" },
    { "AssetRets", "expected asset returns within holding period", "XLF_OPER" },
    { "AssetVols", "volatility of returns within holding period", "XLF_OPER" },
    { "CorrelMat", "asset return correlation matrix", "XLF_OPER" },
    { "Headers", "TRUE for displaying the header", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfPtRisk(
    "xlOrfPtRisk", "ORF.PTRISK", "Mean return and standard deviation of a portfolio.",
    "ORFLIB", OrfPtRiskArgs, 5);

  // Register the function ORF.MVPWGHTS
  XLRegistration::Arg OrfMvpWghtsArgs[] = {
    { "AssetRets", "expected asset returns within holding period", "XLF_OPER" },
    { "AssetVols", "volatility of returns within holding period", "XLF_OPER" },
    { "CorrelMat", "asset return correlation matrix", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfMvpWghts(
    "xlOrfMvpWghts", "ORF.MVPWGHTS", "Weights of the minimum variance portfolio.",
    "ORFLIB", OrfMvpWghtsArgs, 3);

  // Register the function ORF.MEANVARWGHTS
  XLRegistration::Arg OrfMeanVarWghtsArgs[] = {
	{ "AssetRets", "expected asset returns within holding period", "XLF_OPER" },
	{ "AssetVols", "volatility of returns within holding period", "XLF_OPER" },
	{ "CorrelMat", "asset return correlation matrix", "XLF_OPER" },
	{ "RiskAversion", "risk aversion", "XLF_OPER"}
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfMeanVarWghts(
	  "xlOrfMeanVarWghts", "ORF.MEANVARWGHTS", "Weights of the mean-variance optimal portfolio.",
	  "ORFLIB", OrfMeanVarWghtsArgs, 4);

  // Register the function ORF.MEANVARFRONT
  XLRegistration::Arg OrfMeanVarFrontArgs[] = {
	{ "AssetRets", "expected asset returns within holding period", "XLF_OPER" },
	{ "AssetVols", "volatility of returns within holding period", "XLF_OPER" },
	{ "CorrelMat", "asset return correlation matrix", "XLF_OPER" },
	{ "LambdaMax", "max risk aversion", "XLF_OPER"},
	{ "NLambdaSteps", "steps of risk aversion", "XLF_OPER"},
	{ "Headers", "TRUE for displaying the header", "XLF_OPER"}
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfMeanVarFront(
	  "xlOrfMeanVarFront", "ORF.MEANVARFRONT", "Frontier of the mean-variance optimal portfolio.",
	  "ORFLIB", OrfMeanVarWghtsArgs, 6);

  // Register the function ORF.MKTWGHTS
  XLRegistration::Arg OrfMktWghtsArgs[] = {
    { "AssetRets", "expected asset returns within holding period", "XLF_OPER" },
    { "AssetVols", "volatility of returns within holding period", "XLF_OPER" },
    { "CorrelMat", "asset return correlation matrix", "XLF_OPER" },
    { "RFreeRate", "risk free rate of return", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfMktWghts(
    "xlOrfMktWghts", "ORF.MKTWGHTS", "Weights of the CAPM market portfolio.",
    "ORFLIB", OrfMktWghtsArgs, 4);

  // Register the function ORF.MKTRISK
  XLRegistration::Arg OrfMktRiskArgs[] = {
    { "AssetRets", "expected asset returns within holding period", "XLF_OPER" },
    { "AssetVols", "volatility of returns within holding period", "XLF_OPER" },
    { "CorrelMat", "asset return correlation matrix", "XLF_OPER" },
    { "RFreeRate", "risk free rate of return", "XLF_OPER" },
    { "Headers", "TRUE for displaying the header", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfMktRisk(
    "xlOrfMktRisk", "ORF.MKTRISK", "Mean return, std. deviation and lambda of the CAPM market portfolio.",
    "ORFLIB", OrfMktRiskArgs, 5);

}  // anonymous namespace
