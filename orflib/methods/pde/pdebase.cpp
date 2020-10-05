/**
@file  pdebase.cpp
@brief Implementation of some PdeBase methods
*/

#include <orflib/methods/pde/pdebase.hpp>

BEGIN_NAMESPACE(orf)

/** The entry point for every PDE solver
*/
void PdeBase::solve(PdeParams const& params)
{
  // store the Theta
  theta_ = params.theta;
  // get the time steps
  spprod_->timeSteps(params.nTimeSteps, timesteps_, stepindex_);
  nSteps_ = timesteps_.size();
  

  // initialize the grid
  double T = timesteps_.back();
  initGrid(T, params);

  // compute the conditional forward factors from step to step
  // the row index is the time, the column index is the asset
  Matrix fwdFactors(nSteps_, nAssets_);
  for (size_t j = 0; j < nAssets_; ++j) {
    SPtrYieldCurve spyc = spaccrycs_[j];
    double divyld = divyields_[j];
    for (size_t i = 0; i < nSteps_ - 1; ++i) {
      double T1 = timesteps_[i];
      double T2 = timesteps_[i + 1];
      double fwdRate = spyc->fwdRate(T1, T2);
      fwdFactors(i, j) = exp((fwdRate - divyld) * (T2 - T1));
    }
  }

  // compute the forward vols from step to step
  Matrix fwdVols(nSteps_, nAssets_);
  for (size_t j = 0; j < nAssets_; ++j) {
    for (size_t i = 0; i < nSteps_ - 1; ++i) {
      double T1 = timesteps_[i];
      double T2 = timesteps_[i + 1];
      fwdVols(i, j) = vols_[j]->fwdVol(T1, T2);
    } 
  }

  // initialize the value layers (grid functions, one per variable to solve)
  initValLayers();

  // evaluate the product at maturity
  evalProduct(nSteps_ - 1);

  // the main loop
  for (ptrdiff_t stepIdx = nSteps_ - 2; stepIdx >= 0; --stepIdx) {
    updateGrid(params, fwdFactors, fwdVols, stepIdx);

    // solve
    double dT = timesteps_[stepIdx + 1] - timesteps_[stepIdx];
    solveFromStepToStep(stepIdx, dT);

    // discount
    double df = spdiscyc_->fwdDiscount(timesteps_[stepIdx], timesteps_[stepIdx + 1]);
    discountFromStepToStep(df);

    // eval product for next iteration
    evalProduct(stepIdx);
  }
  storeResults();
}

/** Initializes the grid axes, sets up the nodes and the bounds
*/
void PdeBase::initGrid(double T, PdeParams const& params)
{
  ORF_ASSERT(nAssets_ == params.nSpotNodes.size(),
    "PdeBase: unequal number of assets and pde parameter axes specs!");
  // Resize the grid; the number of axes is equal to the number of equities in the product
  resize(nAssets_);

  // loop over assets
  for (size_t i = 0; i < nAssets_; ++i) {
    GridAxis& grax = gridAxes_[i];
    grax.NX = params.nSpotNodes[i];
    double S0 = spots_[i];
    spotAxis_.push_back(S0);

    // compute forward to maturity
    double rate = spaccrycs_[i]->spotRate(T);
    double forward = S0 * exp((rate - divyields_[i]) * T);
    double vol = vols_[i]->spotVol(T);

    // initialize the coordinate transform for this axis
    grax.coordinateChange->init(params);
    double X0 = grax.coordinateChange->fromRealToDiffused(S0);
    double forwardX = forward;
    double volX = vol;
    grax.coordinateChange->forwardAndVariance(forwardX, volX, T);
    grax.coordinateChange->bounds(X0, forwardX, volX, T, params.nStdDevs[i],
      grax.Xmin, grax.Xmax);
    grax.DX = (grax.Xmax - grax.Xmin) / (grax.NX + 1);

    // align the grid axis so that a node passes through the alignment value
    double alignValue = grax.coordinateChange->fromRealToDiffused(alignments_[i]);
    int X0NodeIdx = int(0.5 + (alignValue - grax.Xmin) / grax.DX);
    double closestX = grax.Xmin + X0NodeIdx * grax.DX;
    grax.Xmin -= closestX - alignValue;
    grax.Xmax -= closestX - alignValue;

    // fill in the original and the transformed spot nodes
    grax.Xlevels.resize(params.nSpotNodes[i] + 2);  // add 2 for the boundary nodes
    grax.Slevels.resize(params.nSpotNodes[i] + 2);
    for (size_t j = 0; j <= params.nSpotNodes[i] + 1; ++j) {
      grax.Xlevels[j] = grax.Xmin + j * grax.DX;
      grax.Slevels[j] = grax.coordinateChange->fromDiffusedToReal(grax.Xlevels[j]);
    }

    // resize the drift, variance and vol vectors
    // no need to add boundary points here
    grax.drifts.resize(params.nSpotNodes[i]);
    grax.variances.resize(params.nSpotNodes[i]);
    grax.vols.resize(params.nSpotNodes[i]);
  }
}

/** Updates the grid axes for this time step index */
void PdeBase::updateGrid(PdeParams const& params,
                         Matrix const& fwdFactors,
                         Matrix const& fvols,
                         size_t stepIdx)
{
  double T1 = timesteps_[stepIdx];
  double T2 = timesteps_[stepIdx + 1];
  double DT = T2 - T1;

  for (size_t assetIdx = 0; assetIdx < nAssets_; ++assetIdx) {
    for (size_t j = 1; j <= params.nSpotNodes[assetIdx]; ++j) {
      GridAxis& grax = gridAxes_[assetIdx];
      double RealS = grax.Slevels[j];
      double aCoeff = fwdFactors(stepIdx, assetIdx);
      double RealF = RealS * aCoeff;
      //changed for step localvol
      double RealLNvol = fvols(stepIdx, assetIdx);
      // set the drift, variance and vol values for this time step
      grax.coordinateChange->driftAndVariance(RealS, RealF, theta_, DT, RealLNvol,
        aCoeff, grax.DX, grax.drifts[j - 1], grax.variances[j - 1], grax.vols[j - 1]);
    }
  }
}

END_NAMESPACE(orf)
