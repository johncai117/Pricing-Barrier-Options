/** 
@file  xlregister0.cpp
@brief Registration of Excel callable functions
*/

#include <xlw/xlw.h>
using namespace xlw;

namespace {

// Register the function ORF.VERSION
XLRegistration::Arg OrfVersionArgs[] = {
  { }
};
XLRegistration::XLFunctionRegistrationHelper regOrfVersion(
  "xlOrfVersion", "ORF.VERSION", "The orflib version string.",
  "ORFLIB", OrfVersionArgs, 0);

// Register the function ORF.MATRIXECHO
XLRegistration::Arg OrfMatrixEchoArgs[] = {
  { "Matrix", "Input matrix", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfMatrixEcho(
  "xlOrfMatrixEcho", "ORF.MATRIXECHO", "Echoing the input matrix",
  "ORFLIB", OrfMatrixEchoArgs, 1);

// Register the function ORF.CHOLDCMP
XLRegistration::Arg OrfCholDcmpArgs[] = {
  { "Matrix", "Input matrix", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfCholDcmp(
  "xlOrfCholDcmp", "ORF.CHOLDCMP", "Cholesky decomposition of symmetric, positive definite matrix",
  "ORFLIB", OrfCholDcmpArgs, 1);

// Register the function ORF.EIGENSYM
XLRegistration::Arg OrfEigenSymArgs[] = {
  { "Matrix", "Input matrix", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfEigenSym(
  "xlOrfEigenSym", "ORF.EIGENSYM", "Eigenvalues and eigenvectors of a symmetric matrix",
  "ORFLIB", OrfEigenSymArgs, 1);

// Register the function ORF.SPECTRUNC
XLRegistration::Arg OrfSpecTruncArgs[] = {
  { "Matrix", "Input matrix", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfSpecTrunc(
  "xlOrfSpecTrunc", "ORF.SPECTRUNC", "Spectral truncation of the input correlation matrix",
  "ORFLIB", OrfSpecTruncArgs, 1);

// Register the function ORF.PPOLYEVAL
XLRegistration::Arg OrfPPolyEvalArgs[] = {
  { "BkPoints", "vector of breakpoints", "XLF_OPER" },
  { "Values", "vector of values", "XLF_OPER" },
  { "PolyOrder", "0: piecewise flat; 1: linear continuous", "XLF_OPER" },
  { "X", "Vector of x-vals at which the curve is evaluated", "XLF_OPER" },
  { "DerivOrder", "the order of the derivative to be evaluated; 0 for values", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfPPolyEval(
  "xlOrfPPolyEval", "ORF.PPOLYEVAL", "Creates and evaluates a piecewise polynomial curve ",
  "ORFLIB", OrfPPolyEvalArgs, 5);

// Register the function ORF.PPOLYINTEGRAL
XLRegistration::Arg OrfPPolyIntegralArgs[] = {
  { "BkPoints", "vector of breakpoints", "XLF_OPER" },
  { "Values", "vector of values", "XLF_OPER" },
  { "PolyOrder", "0: piecewise flat; 1: linear continuous", "XLF_OPER" },
  { "XStart", "The x value at which every integration starts", "XLF_OPER" },
  { "XEnd", "Vector of x-vals at which each integration ends", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfPPolyIntegral(
  "xlOrfPPolyIntegral", "ORF.PPOLYINTEGRAL", "Creates and integrates a piecewise polynomial curve ",
  "ORFLIB", OrfPPolyIntegralArgs, 5);

// Register the function ORF.PPOLYSUM
XLRegistration::Arg OrfPPolySumArgs[] = {
  { "BkPoints1", "First vector of breakpoints", "XLF_OPER" },
  { "Values1", "First vector of values", "XLF_OPER" },
  { "BkPoints2", "Second vector of breakpoints", "XLF_OPER" },
  { "Values2", "Second vector of values", "XLF_OPER" },
  { "PolyOrder", "Common ppoly order; 0: piecewise flat; 1: linear continuous", "XLF_OPER" },
};
XLRegistration::XLFunctionRegistrationHelper regOrfPPolySum(
  "xlOrfPPolySum", "ORF.PPOLYSUM", "Creates two polynomial curves and adds them; returns bkpts and values ",
  "ORFLIB", OrfPPolySumArgs, 5);

// Register the function ORF.POLYBRACKET
XLRegistration::Arg OrfPolyBracketArgs[] = {
  { "Coefficients", "coefficients of polynomial", "XLF_OPER" },
  { "LowerLimit", "left boundary of interval", "XLF_OPER" },
  { "UpperLimit", "right boundary of interval", "XLF_OPER" },
  { "NSubs", "number of subintevals to search", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfPolyBracket(
  "xlOrfPolyBracket", "ORF.POLYBRACKET", "Returns the root bracketing intervals of a polynomial",
  "ORFLIB", OrfPolyBracketArgs, 4);

// Register the function ORF.POLYSECANT
XLRegistration::Arg OrfPolySecantArgs[] = {
  { "Coefficients", "coefficients of polynomial", "XLF_OPER" },
  { "LowerLimit", "left boundary of interval", "XLF_OPER" },
  { "UpperLimit", "right boundary of interval", "XLF_OPER" },
  { "Tolerance", "accuracy of root", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfPolySecant(
  "xlOrfPolySecant", "ORF.POLYSECANT", "Returns the root of a polynomial within a bracketing interval",
  "ORFLIB", OrfPolySecantArgs, 4);

// Register the function ORF.TOCC
XLRegistration::Arg OrfToContCmpdArgs[] = {
  { "Rate", "Periodically compounded rate", "XLF_OPER" },
  { "CmpdFreq", "Annual compounding frequency", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfToContCmpd(
  "xlOrfToContCmpd", "ORF.TOCC", "Converts rate from periodic to continously compounded",
  "ORFLIB", OrfToContCmpdArgs, 2);

// Register the function ORF.FROMCC
XLRegistration::Arg OrfFromContCmpdArgs[] = {
  { "Rate", "Continously compounded rate", "XLF_OPER" },
  { "CmpdFreq", "Annual compounding frequency", "XLF_OPER" }
};
XLRegistration::XLFunctionRegistrationHelper regOrfFromContCmpd(
  "xlOrfFromContCmpd", "ORF.FROMCC", "Converts rate from continously to periodic compounded",
  "ORFLIB", OrfFromContCmpdArgs, 2);

}
