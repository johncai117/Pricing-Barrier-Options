/**
@file  bsmcpricer.cpp
@brief Implementation of the BsMcPricer class
*/

#include <orflib/pricers/bsmcpricer.hpp>
#include <orflib/methods/montecarlo/eulerpathgenerator.hpp>

#include <cmath>

using namespace std;

BEGIN_NAMESPACE(orf)

BsMcPricer::BsMcPricer(SPtrProduct prod,
                       SPtrYieldCurve discountCurve,
                       double divYield,
                       double volatility,
                       double spot,
                       McParams mcparams)
: prod_(prod), discyc_(discountCurve), divyld_(divYield), vol_(volatility),
spot_(spot), mcparams_(mcparams)
{
  // Get the simulation times
  Vector timesteps = prod->fixTimes();
  size_t ntimesteps = timesteps.size();

  // Create the path generator, one factor to simulate the spot
  if (mcparams.pathGenType == McParams::PathGenType::EULER) {
    if (mcparams.urngType == McParams::UrngType::MINSTDRAND)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngMinStdRand>(
          timesteps.begin(), timesteps.end(), 1));
    else if (mcparams.urngType == McParams::UrngType::MT19937)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngMt19937>(
          timesteps.begin(), timesteps.end(), 1));
    else if (mcparams.urngType == McParams::UrngType::RANLUX3)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngRanLux3>(
          timesteps.begin(), timesteps.end(), 1));
    else if (mcparams.urngType == McParams::UrngType::RANLUX4)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngRanLux4>(
          timesteps.begin(), timesteps.end(), 1));
    else if (mcparams.urngType == McParams::UrngType::SOBOL)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngSobol>(
          timesteps.begin(), timesteps.end(), 1));
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
  double t1 = 0.0;
  drifts_.resize(fixtimes.size());
  stdevs_.resize(fixtimes.size());
  for (size_t i = 0; i < fixtimes.size(); ++i) {
    double t2 = fixtimes[i];
    double var = vol_ * vol_ * (t2 - t1);
    stdevs_[i] = sqrt(var);
    double fwdrate = discyc_->fwdRate(t1, t2);
    // risk free rate less yield plus convexity adjustment
    drifts_[i] = (fwdrate - divyld_) * (t2 - t1) - 0.5 * var;
    t1 = t2;
  }

  // Resize the payment amounts
  payamts_.resize(prod->payTimes().size());

}


double BsMcPricer::processOnePath(Matrix& pricePath)
{
  pathgen_->next(pricePath);
  // convert the normal deviates to a price path in-place
  double spot = spot_;
  for (size_t i = 0; i < pricePath.n_rows; ++i) {
    double normaldeviate = pricePath(i, 0);
    pricePath(i, 0) = spot * exp(drifts_[i] + stdevs_[i] * normaldeviate);
    spot = pricePath(i, 0);
  }
  prod_->eval(pricePath);
  payamts_ = prod_->payAmounts();

  double pv = 0.0;
  for (size_t i = 0; i < payamts_.size(); ++i)
    pv += discfactors_[i] * payamts_[i];

  return pv;
}

END_NAMESPACE(orf)
