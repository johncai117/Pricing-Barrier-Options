/**
@file  asianbasketcallput.hpp
@brief The payoff of an Asian Call/Put option on a basket of assets
*/

#ifndef ORF_ASIANBASKETCALLPUT_HPP
#define ORF_ASIANBASKETCALLPUT_HPP

#include <orflib/products/product.hpp>
#include <algorithm>
#include <functional>

BEGIN_NAMESPACE(orf)

/** The Asian basket call/put class
*/
class AsianBasketCallPut : public Product
{
public:
  /** Initializing ctor */
  AsianBasketCallPut(int payoffType,
                     double strike,
                     Vector const& fixingTimes,
                     Vector const& assetQuantities);

  /** The number of assets this product depends on */
  virtual size_t nAssets() const override;

  /** Evaluates the product given the passed-in path
      The "pricePath" matrix must have as many rows as
      the number of fixing times
      */
  virtual void eval(Matrix const& pricePath) override;

  /** Evaluates the product at fixing time index idx
  */
  virtual void eval(size_t idx, Vector const& spots, double contValue) override;

private:
  int payoffType_;          // 1: call; -1 put
  double strike_;
  Vector assetQuantities_;  // number of units of each asset in the basket
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
AsianBasketCallPut::AsianBasketCallPut(int payoffType,
                                       double strike,
                                       Vector const& fixingTimes,
                                       Vector const& assetQuantities)
: payoffType_(payoffType), strike_(strike), assetQuantities_(assetQuantities)
{
  ORF_ASSERT(payoffType == 1 || payoffType == -1, "AsianBasketCallPut: the payoff type must be 1 (call) or -1 (put)!");
  ORF_ASSERT(strike >= 0.0, "AsianBasketCallPut: the strike must be positive!");
  ORF_ASSERT(fixingTimes[0] >= 0.0,
    "AsianBasketCallPut: the first fixing time must be non-negative!");
  Vector::const_iterator it(
    std::adjacent_find(fixingTimes.begin(), fixingTimes.end(), std::greater_equal<double>()));
  ORF_ASSERT(it == fixingTimes.end(),
    "AsianBasketCallPut: the fixing times must be in strict increasing order");

  // set the fixing times
  fixTimes_ = fixingTimes;
  // assume that it will settle (pay) at expiration
  // TODO allow payment time later than expiration
  payTimes_.resize(1);
  payTimes_[0] = fixingTimes[fixingTimes.size() - 1];

  // this product generates only one payment
  payAmounts_.resize(1);
}

inline
size_t AsianBasketCallPut::nAssets() const
{
  return assetQuantities_.size();
}

inline void AsianBasketCallPut::eval(Matrix const& pricePath)
{
  double bsktAvg = 0;
  size_t nfixings = pricePath.n_rows;
  ORF_ASSERT(fixTimes_.size() == nfixings,
    "AsianBasketCallPut: number of fixings mismatch in price path!");
  size_t nassets = pricePath.n_cols;
  ORF_ASSERT(assetQuantities_.size() == nassets,
    "AsianBasketCallPut: number of assets mismatch in price path!");

  for (size_t i = 0; i < nfixings; ++i) {
    double bsktval = 0.0;
    for (size_t j = 0; j < nassets; ++j) {
      bsktval += assetQuantities_[j] * pricePath(i, j);
    }
    bsktAvg += bsktval;
  }
  bsktAvg /= nfixings;

  if (payoffType_ == 1)
    payAmounts_[0] = bsktAvg >= strike_ ? bsktAvg - strike_ : 0.0;
  else
    payAmounts_[0] = bsktAvg >= strike_ ? 0.0 : strike_ - bsktAvg;
}

// Not implemented
inline void AsianBasketCallPut::eval(size_t idx, Vector const& spots, double contValue)
{
  ORF_ASSERT(0, "not implemented!");
}

END_NAMESPACE(orf)

#endif // ORF_ASIANBASKETCALLPUT_HPP
