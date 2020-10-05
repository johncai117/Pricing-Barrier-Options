/**
  @file  multiassetbsmcpricer.cpp
  @brief Implementation of the MultiAssetBsMcPricer class
*/

#include <orflib/pricers/multiassetbsmcpricer.hpp>
#include <orflib/methods/montecarlo/eulerpathgenerator.hpp>

#include <cmath>

using namespace std;

BEGIN_NAMESPACE(orf)

MultiAssetBsMcPricer::MultiAssetBsMcPricer(SPtrProduct prod,
                                           SPtrYieldCurve discountCurve,
                                           Vector const& divYields,
                                           Vector const& volatilities,
                                           Vector const& spots,
                                           Matrix const& correlMatrix,
                                           McParams const& mcparams)
: prod_(prod), discyc_(discountCurve), divylds_(divYields), vols_(volatilities),
spots_(spots), mcparams_(mcparams)
{
  // Get the simulation times
  Vector timesteps = prod->fixTimes();
  size_t ntimesteps = timesteps.size();

  // Get the number of assets (factors) and check inputs for size.
  size_t nassets = prod->nAssets();
  ORF_ASSERT(divYields.size() == nassets, "need as many div yields as product assets!");
  ORF_ASSERT(volatilities.size() == nassets, "need as many volatilities as product assets!");
  ORF_ASSERT(spots.size() == nassets, "need as many spots as product assets!");
  if (nassets > 1) {
    ORF_ASSERT(correlMatrix.is_square(), "the correlation matrix must be square!");
    ORF_ASSERT(correlMatrix.n_rows == nassets, "need as many correlation matrix rows as product assets!");
  }

  // Create the path generator, one factor to simulate the spot
  if (mcparams.pathGenType == McParams::PathGenType::EULER) {
    if (mcparams.urngType == McParams::UrngType::MINSTDRAND)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngMinStdRand>(
        timesteps.begin(), timesteps.end(), nassets, correlMatrix));
    else if (mcparams.urngType == McParams::UrngType::MT19937)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngMt19937>(
        timesteps.begin(), timesteps.end(), nassets, correlMatrix));
    else if (mcparams.urngType == McParams::UrngType::RANLUX3)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngRanLux3>(
        timesteps.begin(), timesteps.end(), nassets, correlMatrix));
    else if (mcparams.urngType == McParams::UrngType::RANLUX4)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngRanLux4>(
        timesteps.begin(), timesteps.end(), nassets, correlMatrix));
    else if (mcparams.urngType == McParams::UrngType::SOBOL)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngSobol>(
        timesteps.begin(), timesteps.end(), nassets, correlMatrix));
    else
      ORF_ASSERT(0, "unknown urng type!");
  }
  else
    ORF_ASSERT(0, "unknown path generator type!");

  // Pre-compute the discount factors
  Vector const& paytimes = prod->payTimes();
  discfactors_.resize(paytimes.size());
  for (size_t i = 0; i < paytimes.size(); ++i)
    discfactors_[i] = discyc_->discount(paytimes[i]);

  // Pre-compute the stdevs and drifts from time step to time step
  Vector const& fixtimes = prod->fixTimes();
  drifts_.resize(fixtimes.size(), nassets);
  stdevs_.resize(fixtimes.size(), nassets);

  // loop over assets
  for (size_t j = 0; j < nassets; ++j) {
    double t1 = 0.0;
    // loop over fixing times
    for (size_t i = 0; i < fixtimes.size(); ++i) {
      double t2 = fixtimes[i];
      double var = vols_[j] * vols_[j] * (t2 - t1);
      stdevs_(i, j) = vols_[j];
      double fwdrate = discyc_->fwdRate(t1, t2);
      // risk free rate less yield plus convexity adjustment
      drifts_(i, j) = (fwdrate - divylds_[j]) * (t2 - t1) - 0.5 * var;
      t1 = t2;
    }
  }

  // Resize the payment amounts
  payamts_.resize(prod->payTimes().size());

}

double MultiAssetBsMcPricer::processOnePath(Matrix& pricePath)
{
  pathgen_->next(pricePath);
  size_t nassets = prod_->nAssets();
  currspots_ = spots_;               // initialize the current spots array
  // convert the normal deviates to a price path in-place
  for (size_t i = 0; i < pricePath.n_rows; ++i) {
    for (size_t j = 0; j < nassets; ++j) {
      double normaldeviate = pricePath(i, j);
      pricePath(i, j) = currspots_[j] * exp(drifts_(i, j) + stdevs_(i, j) * normaldeviate);
      currspots_[j] = pricePath(i, j);  // store the spot for the next time step
    }
  }
  prod_->eval(pricePath);
  payamts_ = prod_->payAmounts();

  double pv = 0.0;
  for (size_t i = 0; i < payamts_.size(); ++i)
    pv += discfactors_[i] * payamts_[i];

  return pv;
}

END_NAMESPACE(orf)
