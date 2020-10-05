/**
@file  pde1dsolver.cpp
@brief Implementation of the 1-dim PDE solver class
*/

#include <orflib/methods/pde/pde1dsolver.hpp>
#include <orflib/math/interpol/interpolation1d.hpp>

BEGIN_NAMESPACE(orf)

/** Stores the solver results */
void Pde1DSolver::setAlignment(bool setAlignmenttoBarrier)
{
	// set the alignment values to the corresponding spots/ barriers depending on whether setAlignmnet to Barrier is true or false
	if (setAlignmenttoBarrier == false) {
		alignments_ = spots_;
	}
	else {
		alignments_ = barriers_;
	}
}


/** Solves backwards from one time step to the previous */
void Pde1DSolver::solveFromStepToStep(ptrdiff_t step, double DT)
{
  // initialise operators
  GridAxis& grax = gridAxes_[0];
  deltaOpExplicit_.init(grax.drifts, DT, grax.DX, 1.0 - theta_);
  deltaOpImplicit_.init(grax.drifts, DT, grax.DX, theta_);

  gammaOpExplicit_.init(grax.variances, DT, grax.DX, 1.0 - theta_);
  gammaOpImplicit_.init(grax.variances, DT, grax.DX, theta_);

  // build the explicit and implicit operators
  opExplicit_.init(grax.NX, 0.0, 1.0, 0.0); // initialize to identity matrix
  opExplicit_ += deltaOpExplicit_;
  opExplicit_ += gammaOpExplicit_;
  opImplicit_.init(grax.NX, 0.0, 1.0, 0.0); // initialize to identity matrix
  opImplicit_ -= deltaOpImplicit_;
  opImplicit_ -= gammaOpImplicit_;

  // adjust the operators for boundary conditions
  adjustOpsForBoundaryConditions(opExplicit_, opImplicit_, grax.DX);

  // Main loop over the layers
  for (size_t j = 0; j < nLayers_; ++j) {
    // NOTE: v1 and v2 are read-write views into the corresponding columns
    // They are not independent copies, so we are modifying in place prevValues and currValues
    auto v1 = prevValues->col(j);
    auto v2 = currValues->col(j);
    opExplicit_.apply(v1, v2);
    opImplicit_.applyInverse(v2, v1);
  }

  // apply boundary coditions to solution
  applyBoundaryConditions(*prevValues);
}


/** Initializes the layers (grid functions) */
void Pde1DSolver::initValLayers()
{
  ORF_ASSERT(nFactors() == 1, "1D PDE handles 1 asset only!");
  values1.resize(gridAxes_[0].NX + 2, nLayers_);
  values2.resize(gridAxes_[0].NX + 2, nLayers_);
  // initialize all values to zero
  for (size_t i = 0; i < gridAxes_[0].NX + 2; ++i) {
    for (size_t j = 0; j < nLayers_; ++j) {
      values1(i, j) = values2(i, j) = 0.0;
    }
  }
  prevValues = &values1; currValues = &values2;

  // prepare the results
  results_.times.resize(nSteps_);
  results_.values.resize(nSteps_);
  if (storeAllResults_)
    results_.values.resize(nSteps_);
}


/** Evaluates the product at the passed-in time step index */
void Pde1DSolver::evalProduct(size_t stepIdx)
{
  ptrdiff_t eventIdx = stepindex_[stepIdx];
  if (eventIdx >= 0) {             // product event, must evaluate
    Vector const & payTms = spprod_->payTimes();       // the payment times
    double fixtime = spprod_->fixTimes()[eventIdx]; // the fixing time
    Vector spots(1);                                   // one underlying spot
    for (size_t node = 0; node <= gridAxes_[0].NX + 1; ++node) {
      spots[0] = gridAxes_[0].Slevels[node];
      spprod_->eval(eventIdx, spots, (*prevValues)(node, 0));
      // read out the ammounts
      Vector const & payAms = spprod_->payAmounts();
      size_t i = 0;
      while (fixtime > payTms[i]) ++i;
      // TODO: fwd discount
      (*prevValues)(node, 0) = payAms[eventIdx];
    }
  }
  results_.times[stepIdx] = timesteps_[stepIdx];
  if (storeAllResults_)
    results_.values[stepIdx] = *prevValues;
}




/** Stores the solver results */
void Pde1DSolver::storeResults()
{
  results_.gridAxes = gridAxes_;
  results_.prices.resize(nLayers_);
  for (size_t j = 0; j < nLayers_; ++j) {
    double X0 = gridAxes_[0].coordinateChange->fromRealToDiffused(spots_[0]);
    Vector temp(prevValues->col(j));
    LinearInterpolation1D<Vector> interp(gridAxes_[0].Xlevels, temp);
    results_.prices[j] = interp.getValue(X0);
  }
}

/** Discounts the grid functions on the current time step, by applying
    the passed-in one-step discount factor. */
void Pde1DSolver::discountFromStepToStep(double df)
{
  *prevValues *= df;
}

END_NAMESPACE(orf)