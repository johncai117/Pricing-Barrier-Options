/**
@file  americancallput.hpp
@brief The payoff of an American Call/Put option
*/

#ifndef ORF_AMERICANCALLPUT_HPP
#define ORF_AMERICANCALLPUT_HPP

#include <orflib/products/europeancallput.hpp>

BEGIN_NAMESPACE(orf)

/** The American call/put class
*/
class AmericanCallPut : public EuropeanCallPut
{
public:
  /** Initializing ctor */
  AmericanCallPut(int payoffType, double strike, double timeToExp);

  /** Evaluates the product at fixing time index idx
  */
  virtual void eval(size_t idx, Vector const& pricePath, double contValue);
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
AmericanCallPut::AmericanCallPut(int payoffType, double strike, double timeToExp)
: EuropeanCallPut(payoffType, strike, timeToExp)
{
  // count the number of days between 0 and timeToExp
  size_t nfixings = static_cast<size_t>(timeToExp * DAYS_PER_YEAR) + 1;
  ORF_ASSERT(nfixings > 0, "AmericanCallPut: the option has expired!");
  fixTimes_.resize(nfixings);
  for (size_t i = 0; i < nfixings - 1; ++i)
    fixTimes_[i] = i / DAYS_PER_YEAR;
  fixTimes_[nfixings - 1] = timeToExp_;

  payTimes_ = fixTimes_;

  // this product could generate a payment on each day between now and expiration.
  payAmounts_.resize(payTimes_.size());
}

// This product has as many fixings as days between 0 and time to expiration.
inline void AmericanCallPut::eval(size_t idx, Vector const& spots, double contValue)
{
  double spot = spots[0];

  if (idx == payAmounts_.size() - 1) { // this is the last index
    double payoff = (spot - strike_) * payoffType_;
    payAmounts_[idx] = payoff > 0.0 ? payoff : 0.0;
  }
  else {  // this is not the last index, check the exercise condition
    double intrinsicValue = (spot - strike_) * payoffType_;
    intrinsicValue = intrinsicValue >= 0.0 ? intrinsicValue : 0.0;
    payAmounts_[idx] = contValue >= intrinsicValue ? contValue : intrinsicValue;
    // zero out the amounts after this index
    for (size_t j = idx + 1; j < payAmounts_.size(); ++j)
      payAmounts_[j] = 0.0;
  }
}

END_NAMESPACE(orf)

#endif // ORF_AMERICANCALLPUT_HPP
