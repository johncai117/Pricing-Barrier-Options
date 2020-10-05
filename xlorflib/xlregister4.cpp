/** 
@file  xlregister4.cpp
@brief Registration of Excel callable functions
*/

#include <xlw/xlw.h>
using namespace xlw;

namespace {

  // Register the function ORF.EUROBSPDE
  XLRegistration::Arg OrfEuroBSPDEArgs[] = {
    { "PayoffType", "1: call; -1: put", "XLF_OPER" },
    { "Strike", "strike", "XLF_OPER" },
    { "TimeToExp", "time to expiration", "XLF_OPER" },
    { "Spot", "spot", "XLF_OPER" },
    { "DiscountCrv", "name of the discount curve", "XLF_OPER" },
    { "DivYield", "dividend yield (cont. cmpd.)", "XLF_OPER" },
    { "Vol", "volatility", "XLF_OPER" },
    { "PdeParams", "The PDE parameters", "XLF_OPER" },
    { "AllResults", "FALSE (default) for price only; TRUE for displaying the full grid of results", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfEuroBSPDE(
    "xlOrfEuroBSPDE", "ORF.EUROBSPDE", "Price of a European option in the Black-Scholes model using PDE.",
    "ORFLIB", OrfEuroBSPDEArgs, 9);

  // Register the function ORF.AMERBSPDE
  XLRegistration::Arg OrfAmerBSPDEArgs[] = {
    { "PayoffType", "1: call; -1: put", "XLF_OPER" },
    { "Strike", "strike", "XLF_OPER" },
    { "TimeToExp", "time to expiration", "XLF_OPER" },
    { "Spot", "spot", "XLF_OPER" },
    { "DiscountCrv", "name of the discount curve", "XLF_OPER" },
    { "DivYield", "dividend yield (cont. cmpd.)", "XLF_OPER" },
    { "Vol", "volatility", "XLF_OPER" },
    { "PdeParams", "The PDE parameters", "XLF_OPER" },
    { "AllResults", "FALSE (default) for price only; TRUE for displaying the full grid of results", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfAmerBSPDE(
    "xlOrfAmerBSPDE", "ORF.AMERBSPDE", "Price of an American option in the Black-Scholes model using PDE.",
    "ORFLIB", OrfAmerBSPDEArgs, 9);

  XLRegistration::Arg OrfBarrBSPDEArgs[] = {
	{ "PayoffType", "1: call; -1: put", "XLF_OPER" },
	{ "Strike", "strike", "XLF_OPER" },
	{ "TimeToExp", "time to expiration", "XLF_OPER" },
	{ "Spot", "spot", "XLF_OPER" },
	{ "Barrier", "barrier", "XLF_OPER" },
	{ "Freq", "observation frequency", "XLF_OPER" },
	{ "BarrType", "barrier type", "XLF_OPER" },
	{ "DiscountCrv", "name of the discount curve", "XLF_OPER" },
	{ "DivYield", "dividend yield (cont. cmpd.)", "XLF_OPER" },
	{ "Vol", "volatility", "XLF_OPER" },
	{ "PdeParams", "The PDE parameters", "XLF_OPER" },
	{ "SetAlignmentoBarr", "FALSE (default) for setting alignment to spot; TRUE for setting alignment to barrier", "XLF_OPER" },
	{ "AllResults", "FALSE (default) for price only; TRUE for displaying the full grid of results", "XLF_OPER" }
  };
  XLRegistration::XLFunctionRegistrationHelper regOrfBarrBSPDE(
	  "xlOrfBarrBSPDE", "ORF.BARRBSPDE", "Price of a Barrier option in the Black-Scholes model using PDE.",
	  "ORFLIB", OrfBarrBSPDEArgs, 13);

}  // anonymous namespace
