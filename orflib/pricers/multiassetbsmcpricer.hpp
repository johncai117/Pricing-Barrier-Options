/**
@file  multiassetbsmcpricer.hpp
@brief Multiasset Monte Carlo pricer in the Black Scholes model
*/

#ifndef ORF_MULTIASSETBSMCPRICER_HPP
#define ORF_MULTIASSETBSMCPRICER_HPP


#include <orflib/products/product.hpp>
#include <orflib/market/yieldcurve.hpp>
#include <orflib/methods/montecarlo/mcparams.hpp>
#include <orflib/methods/montecarlo/pathgenerator.hpp>
#include <orflib/math/stats/statisticscalculator.hpp>

BEGIN_NAMESPACE(orf)

/** Multiasset Monte Carlo pricer in the Black-Scholes model (deterministic rates and vols).
    Current constraint: all assets must be in the same economy, i.e. share the same yield curve.
    */
class MultiAssetBsMcPricer
{

public:
  /** Initializing ctor */
  MultiAssetBsMcPricer(SPtrProduct prod,
                       SPtrYieldCurve discountYieldCurve,
                       Vector const& divYields,
                       Vector const& volatilities,
                       Vector const& spots,
                       Matrix const& correlMatrix,
                       McParams const& mcparams);

  /** Returns the number of variables that can be tracked for stats */
  size_t nVariables();

  /** Runs the simulation and collects statistics */
  template<typename ITER>
  void simulate(StatisticsCalculator<ITER>& statsCalc, unsigned long npaths);

protected:

  /** Creates and processes one price path.
      It returns the PV of the product
  */
  double processOnePath(Matrix& pricePath);

private:
  SPtrProduct prod_;               // pointer to the product
  SPtrYieldCurve discyc_;          // pointer to the discount curve
  Vector divylds_;                 // the constant dividend yield, one per asset   
  Vector vols_;                    // the constant volatility, one per asset
  Vector spots_;                   // the initial spots, one per asset
  McParams mcparams_;              // the Monte Carlo parameters

  SPtrPathGenerator pathgen_;  // pointer to the path generator
  Vector discfactors_;         // caches the pre-computed discount factors
  Matrix drifts_;              // caches the pre-computed asset drifts, one column per asset
  Matrix stdevs_;              // caches the pre-computed standard deviations, one column per asset 

  Vector currspots_;           // scratch array with the current spots, one per asset
  Vector payamts_;             // scratch array for writting the payments after each simulation
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
size_t MultiAssetBsMcPricer::nVariables()
{
  return 1;  // just one variable, the price
}

template<typename ITER>
void MultiAssetBsMcPricer::simulate(StatisticsCalculator<ITER>& statsCalc, unsigned long npaths)
{
  // create the price path matrix
  Matrix pricePath(pathgen_->nTimeSteps(), pathgen_->nFactors());
  // check the size of the statistics calculator
  ORF_ASSERT(statsCalc.nVariables() == nVariables(), "the statistics calculator must track as many variables as the pricer captures!");

  // This is the HOT loop
  for (unsigned long i = 0; i < npaths; ++i) {
    double pv = processOnePath(pricePath);
    statsCalc.addSample(&pv, &pv + 1);
  }
}

END_NAMESPACE(orf)

#endif // ORF_MULTIASSETBSMCPRICER_HPP
