/**
@file  pde1dsolver.hpp
@brief Definition of the 1-dim PDE solver class
*/

#ifndef ORF_PDE1DSOLVER_HPP
#define ORF_PDE1DSOLVER_HPP

#include <orflib/methods/pde/pdebase.hpp>
#include <orflib/methods/pde/tridiagonalops1d.hpp>
#include <orflib/methods/pde/pderesults.hpp>

BEGIN_NAMESPACE(orf)

/** The 1-d pde solver class */
class Pde1DSolver : public PdeBase
{
public:
  /** Ctor */
  Pde1DSolver(SPtrProduct product,
              SPtrYieldCurve discountYieldCurve,
              double spot,
              double divyield,
              SPtrVolatilityTermStructure vol,
              Pde1DResults& results,
              bool storeAllResults = false,
			  double barrier = 0  ) // make the barrier argument optional, with a placeholder value
  : PdeBase(product), results_(results), storeAllResults_(storeAllResults)
  {
    nAssets_ = product->nAssets();
    nLayers_ = 1;  // currently we solve for only one variable
    spdiscyc_ = discountYieldCurve;
	spots_.push_back(spot),
	barriers_.push_back(barrier),
    spaccrycs_.push_back(discountYieldCurve);
    divyields_.push_back(divyield);
	vols_.push_back(vol);
  }

  /** Dtor */
  virtual ~Pde1DSolver() override {}

  /** Set alignment method */
  virtual void setAlignment(bool setAlignmenttoBarrier);

  /** Solves backwards from one time step to the previous */
  virtual void solveFromStepToStep(ptrdiff_t step, double DT) override;

  /** Initializes the layers.
      Each layer (grid function) corresponds to variable solved on the grid.
  */
  virtual void initValLayers() override;

  /** Evaluates the product at the passed-in time step index */
  virtual void evalProduct(size_t stepIdx);

  /** Stores the solver results */
  virtual void storeResults();

  /** Discounts the grid functions on the current time step, by applying
      the passed-in one-step discount factor. */
  virtual void discountFromStepToStep(double df);

protected:

  //state
  DeltaOp1D<Vector> deltaOpExplicit_, deltaOpImplicit_;
  GammaOp1D<Vector> gammaOpExplicit_, gammaOpImplicit_;
  TridiagonalOp1D<Vector> opExplicit_, opImplicit_;
  

  bool storeAllResults_;
  Pde1DResults& results_;
  

  Matrix values1, values2;  // each row corresponds to a spot node, each column to a variable
  Matrix* prevValues, * currValues;

};

END_NAMESPACE(orf)

#endif  // #ifndef ORF_PDE1DSOLVER_HPP
