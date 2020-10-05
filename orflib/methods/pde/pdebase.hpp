/**
@file  pdebase.hpp
@brief Definition of the PdeBase abstract base class
*/

#ifndef ORF_PDEBASE_HPP
#define ORF_PDEBASE_HPP

#include <orflib/methods/pde/pdegrid.hpp>
#include <orflib/methods/pde/pdeparams.hpp>
#include <orflib/products/product.hpp>
#include <orflib/market/yieldcurve.hpp>
#include <orflib/market/volatilitytermstructure.hpp>

#include <vector>

BEGIN_NAMESPACE(orf)

/**
Abstract base class for all PDE solvers;
It implements the non-virual method solve() that runs the solver.
Various steps inside the solver correspond to pure virtual methods that must be
overriden in the derived classes.
The class also provides implementation for the initGrid() and updateGrid() methods,
which may be overriden in derived classes.
*/
class PdeBase
{
public:

  /** Ctor from product and market data */
  PdeBase(SPtrProduct product,
    SPtrYieldCurve discountYieldCurve,
    std::vector<double> spots,
    std::vector<SPtrYieldCurve> const & accrualYieldCurves,
    std::vector<double> divyields,
    std::vector<SPtrVolatilityTermStructure> vols)
    : spprod_(product), spdiscyc_(discountYieldCurve), spots_(spots),
    spaccrycs_(accrualYieldCurves), divyields_(divyields), vols_(vols)
  {
    nAssets_ = spprod_->nAssets();
    ORF_ASSERT(spots_.size() == nAssets_, "missing or redundant spots!");
    ORF_ASSERT(spaccrycs_.size() == nAssets_, "missing or redundant accrual yield curves!");
    ORF_ASSERT(divyields_.size() == nAssets_, "missing or redundant dividend yields!");
    ORF_ASSERT(vols_.size() == nAssets_, "missing or redundant volatilities!");
  }

  /** Dtor */
  virtual ~PdeBase() {}

  /** Returns the number of factors, i.e. number of dynamic variables.
      This is equal to the number of grid spatial (asset) dimensions.
   */
  size_t nFactors() const { return gridAxes_.size(); }

  /** Resizes the grid */
  void resize(size_t nEq)
  {
    gridAxes_.resize(nEq);
  }

  /** The entry point for the solver; this is the method that the client needs to call */
  void solve(PdeParams const& params);

  /** Initializes the grid axes, sets up the nodes and the bounds */
  virtual void initGrid(double T, PdeParams const& params);

  /** Updates the drift and variance coefficients for the current time step */
  virtual void updateGrid(PdeParams const& params,
                          Matrix const& fwdFactors,
                          Matrix const& fwdVols,
                          size_t stepIdx);

  /** Solves backwards from one time step to the previous */
  virtual void solveFromStepToStep(ptrdiff_t step, double DT) = 0;

  /** Initializes the layers (grid functions) */
  virtual void initValLayers() = 0;

  /** Evaluates the product at the passed-in time step index */
  virtual void evalProduct(size_t stepIdx) = 0;

  /** Stores the solver results */
  virtual void storeResults() = 0;

  /** Discounts the grid functions on the current time step, by applying
      the passed-in one-step discount factor. */
  virtual void discountFromStepToStep(double df) = 0;

protected:
  /** Default ctor */
  PdeBase() {}

  /** Ctor from product; inherited classes must set the other market data */
  PdeBase(SPtrProduct product) : spprod_(product) {}

  // state
  size_t nSteps_;                     // number of times steps
  size_t nAssets_;                    // number of assets to diffuse
  size_t nLayers_;                    // number of PDE variables being solved on the same grid
  double theta_;

  SPtrProduct spprod_;                       // the product being priced
  SPtrYieldCurve spdiscyc_;                  // the discounting yield curve 
  std::vector<double> spots_;                // the asset spots
  std::vector<double> barriers_;                // the asset barriers
  std::vector<SPtrYieldCurve> spaccrycs_;    // the accrual yield curve (used for forward calculation)
  std::vector<double> divyields_;            // the dividend yields for each asset
  std::vector<SPtrVolatilityTermStructure> vols_;  // the volatility term structure for each asset

  std::vector<GridAxis> gridAxes_;  // the grid axes
  std::vector<double> spotAxis_;
  std::vector<double> alignments_;  // one value per axis at which a grid node must pass through
  std::vector<double> timesteps_;   // the vector of time steps
  std::vector<ptrdiff_t> stepindex_;      // the vector of time step indices; of >= 0, product must be evaluated

};

END_NAMESPACE(orf)

#endif  // #ifndef ORF_PDEBASE_HPP
