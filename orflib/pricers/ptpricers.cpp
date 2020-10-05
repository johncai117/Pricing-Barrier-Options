/**
@file  ptpricers.cpp
@brief Implementation of portfolio pricing functions
*/

#include <orflib/pricers/ptpricers.hpp>
#include <orflib/math/linalg/linalg.hpp>
#include <orflib/utils.hpp>
#include <cmath>

using namespace arma;

BEGIN_NAMESPACE(orf)

/** Validates the inputs and returns the number of assets */
size_t validatePtInputs(Vector const& assetRets, Vector const& assetVols, Matrix const& correlMat)
{
  // get the number of assets and check inputs for size
  size_t nassets = assetRets.size();
  ORF_ASSERT(nassets > 0, "must have at least one asset return!");
  ORF_ASSERT(assetVols.size() == nassets, "need as many volatilities as asset returns!");
  for (size_t i = 0; i < nassets; ++i)
    ORF_ASSERT(assetVols[i] > 0.0, "volatilities must be positive!");
  ORF_ASSERT(correlMat.n_rows == nassets, "need as many correlation matrix rows as asset returns!");
  ORF_ASSERT(correlMat.n_cols == nassets, "the correlation matrix must be square!");
  for (size_t i = 0; i < nassets; ++i) {
    for (size_t j = 0; j < i; ++j) {
      ORF_ASSERT(correlMat(i, j) == correlMat(j, i), "the correlation matrix must be symmetric!");
    }
  }
  return nassets;
}

/** The mean return and standard deviation of returns for the portfolio.
    The first element of the tuple is the mean and the second is the stdev
*/
std::tuple<double, double> ptRisk(Vector const& weights,
                                  Vector const& assetRets,
                                  Vector const& assetVols,
                                  Matrix const& correlMat)
{
  size_t nassets = validatePtInputs(assetRets, assetVols, correlMat);
  ORF_ASSERT(weights.size() == nassets, "number of weights must equal the number of asset returns");

  Matrix Sigma = correlMat;
  // convert correlation to covariance matrix
  for (size_t i = 0; i < nassets; ++i)
    for (size_t j = 0; j < nassets; ++j)
      Sigma(i, j) *= assetVols(i) * assetVols(j);

  double meanret = dot(weights, assetRets);
  double ptvar = dot(weights, Sigma * weights);
  double ptvol = sqrt(ptvar);
  return std::make_tuple(meanret, ptvol);
}

/** The weights of the minimum variance portfolio */
Vector mvpWeights(Vector const& assetRets, Vector const& assetVols, Matrix const& correlMat)
{
  size_t nassets = validatePtInputs(assetRets, assetVols, correlMat);
  Vector iota(nassets, fill::ones);
  Matrix SigmaInv = correlMat;
  // convert correlation to covariance matrix
  for (size_t i = 0; i < SigmaInv.n_rows; ++i)
	  for (size_t j = 0; j < SigmaInv.n_cols; ++j) {
		  ORF_ASSERT(SigmaInv(i, j) >= -1, "Correlation must be greater than or equal to -1");
		  ORF_ASSERT(SigmaInv(i, j) <= 1, "Correlation must be smaller than or equal to 1");
		  if (i == j) {
			  ORF_ASSERT(SigmaInv(i, j) == 1, "Diagonals of correlation matrix must be equal to 1");
		  }
		  SigmaInv(i, j) *= assetVols(i) * assetVols(j);
	  }

  SigmaInv = SigmaInv.i();   // invert in place
  Vector wghts = SigmaInv * iota;
  double c = dot(iota, wghts);
  wghts = (1.0 / c) * wghts;
  return wghts;
}

/** The weights of the minimum variance portfolio */
Vector meanvarWeights(Vector const& assetRets, Vector const& assetVols, Matrix const& correlMat, double riskaversion)
{
	size_t nassets = validatePtInputs(assetRets, assetVols, correlMat);
	Vector iota(nassets, fill::ones);
	Matrix SigmaInv = correlMat;
	// convert correlation to covariance matrix
	for (size_t i = 0; i < SigmaInv.n_rows; ++i)
		for (size_t j = 0; j < SigmaInv.n_cols; ++j) {
			ORF_ASSERT(SigmaInv(i,j) >= -1, "Correlation must be greater than or equal to -1");
			ORF_ASSERT(SigmaInv(i, j) <= 1, "Correlation must be smaller than or equal to 1");
			if (i == j) {
				ORF_ASSERT(SigmaInv(i, j) == 1, "Diagonals of correlation matrix must be equal to 1");
			}
			SigmaInv(i, j) *= assetVols(i) * assetVols(j);
		}
	SigmaInv = SigmaInv.i();   // invert in place
	Vector wghts = SigmaInv * iota;
	double c = dot(iota, wghts);
	wghts = (1.0 / c) * wghts;
	Vector term1 = SigmaInv * assetRets;
	double a = dot(iota, term1); 
	double term2 = a/c;
	Vector term3 = SigmaInv * iota;
	Vector lambdaterm = riskaversion * (term1 - (term2 * term3));
	Vector finalwghts = wghts + lambdaterm;

	return finalwghts;
}



/** The weights of the CAPM market portfolio */
Vector mktWeights(Vector const& assetRets, Vector const& assetVols, Matrix const& correlMat, double rfreeRate)
{
  size_t nassets = validatePtInputs(assetRets, assetVols, correlMat);
  Vector iota(nassets, fill::ones);
  Matrix SigmaInv = correlMat;
  // convert to covariance matrix
  for (size_t i = 0; i < SigmaInv.n_rows; ++i)
    for (size_t j = 0; j < SigmaInv.n_cols; ++j)
      SigmaInv(i, j) *= assetVols(i) * assetVols(j);

  SigmaInv = SigmaInv.i();   // invert in place
  Vector wghts = SigmaInv * (assetRets - rfreeRate *iota);
  double lambda_mkt = 1.0 / dot(iota, wghts);
  wghts = lambda_mkt * wghts;
  return wghts;
}

/** The mean return, volatility of return and lambda of the CAPM market portfolio
    The tuple contains these quantities in that order.
*/
std::tuple<double, double, double>
mktRisk(Vector const& assetRets, Vector const& assetVols, Matrix const& correlMat, double rfreeRate)
{
  size_t nassets = validatePtInputs(assetRets, assetVols, correlMat);
  Vector iota(nassets, fill::ones);
  Matrix SigmaInv = correlMat;
  // convert to covariance matrix
  for (size_t i = 0; i < SigmaInv.n_rows; ++i)
    for (size_t j = 0; j < SigmaInv.n_cols; ++j)
      SigmaInv(i, j) *= assetVols(i) * assetVols(j);

  SigmaInv = SigmaInv.i();   // invert in place
  Vector wghts = SigmaInv * (assetRets - rfreeRate * iota);
  double lambda_mkt = 1.0 / dot(iota, wghts);
  double h = dot((assetRets - rfreeRate *iota), wghts);

  double meanret = rfreeRate + lambda_mkt * h;
  double stdevret = lambda_mkt * sqrt(h);
  return std::make_tuple(meanret, stdevret, lambda_mkt);
}

END_NAMESPACE(orf)
