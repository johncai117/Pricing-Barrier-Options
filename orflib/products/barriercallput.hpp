/**
@file  barriercallput.hpp
@brief The payoff of an Barrier Call/Put option
*/

#ifndef ORF_BARRIERCALLPUT_HPP
#define ORF_BARRIERCALLPUT_HPP

#include <orflib/products/europeancallput.hpp>

BEGIN_NAMESPACE(orf)

/** The Barrier call/put class
*/
class BarrierCallPut : public EuropeanCallPut
{
public:
	enum class Freq
	{
		MONTHLY,      // 12/year
		WEEKLY,       // 52/year
		DAILY         // 365/year
	};
	/** Initializing ctor */
	BarrierCallPut(int payoffType, double strike, double timeToExp, int up_or_down, double barrier, Freq freq);

	/** Evaluates the product at fixing time index idx
	*/
	virtual void eval(size_t idx, Vector const& pricePath, double contValue);

private:
	double barrier_;
	Freq freq_;
	int up_or_down_;         // 1: up; 0 down
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
BarrierCallPut::BarrierCallPut(int payoffType, double strike, double timeToExp, int up_or_down, double barrier, Freq freq)
	: EuropeanCallPut(payoffType, strike, timeToExp), up_or_down_(up_or_down), barrier_(barrier), freq_(freq)
{
	ORF_ASSERT(payoffType == 1 || payoffType == -1, "BarrierCallPut: the payoff type must be 1 (call) or -1 (put)!");
	ORF_ASSERT(up_or_down == 1 || up_or_down == 0, "BarrierCallPut: the up_or_down type must be 1 (up) or 0 (down)!");
	ORF_ASSERT(strike > 0.0, "BarrierCallPut: the strike must be positive!");
	ORF_ASSERT(timeToExp > 0.0, "BarrierCallPut: the time to expiration must be positive!");
	
	// switch for the number of days between 0 and timeToExp
	double num_freq;
	switch (freq) {
	case BarrierCallPut::Freq::MONTHLY:
		num_freq = 12;  //12
		break;
	case BarrierCallPut::Freq::WEEKLY:
		num_freq = 52;  // 52
		break;
	case BarrierCallPut::Freq::DAILY:
		num_freq = 365;  //365
		break;
	default:
		ORF_ASSERT(0, "BarrierCallPut: unknown frequency input type");
	}

	//size_t nfixings = static_cast<size_t>(timeToExp * num_freq);
	//Alternative experimental code
	size_t nfixings = static_cast<size_t>(timeToExp * num_freq);
	ORF_ASSERT(nfixings > 0, "BarrierCallPut: the option has expired!");
	fixTimes_.resize(nfixings);
	double stub = (timeToExp * num_freq) - std::floor(timeToExp * num_freq); //compute the size of the stub
	//ORF_ASSERT(stub <= epsilon && stub >= -epsilon, "stub should be zero"); // test code - remember to take out

	ORF_ASSERT(timeToExp >= (1 / num_freq),"Time to Expiration must be greater than the smallest unit of monitoring" );

	
	/*
	// Original code:

	for (size_t i = 0; i < nfixings -1; ++i)  // I think i should be from i = 0 to i < nfixings
		fixTimes_[i] = (i / num_freq) + stub;  // I think this should be stub/num_freq
	fixTimes_[nfixings - 1] = timeToExp_;

	*/

	// Proposed New code:

	for (size_t i = 0; i < nfixings - 1 ; ++i)  // Not sure if changing i < nfixings -1 to < nfixings - 1 is significant
		fixTimes_[i] = (i / num_freq) + (stub/ num_freq);  // I think this should be stub/num_freq
	fixTimes_[nfixings - 1] = timeToExp_;  // this already defines the last element of fixTimes as timeToExp_



	
	// To check with Prof Sotiropoulos: is it i < nfixings -1?
	// Why does American option have nfixings = timeToExp*DaysPerYear + 1?
	// Is the first fixTime at the end of the stub???  <- key question
	// Why is it that fixTimes_[0] for American option is 0? But this is not the case for Bermudan options?
	// Should fixTimes_[0] be 0 (like the start of the period), or like 1 month/1 day into the period?

	payTimes_ = fixTimes_;

	// make this product seem like a american option but modify it so that we don't allow for early exercise
	payAmounts_.resize(payTimes_.size());
	

}

// This product has as many fixings as num_freq between 0 and time to expiration.
inline void BarrierCallPut::eval(size_t idx, Vector const& spots, double contValue)
{
	double spot = spots[0];

	if (idx == payAmounts_.size() - 1) { // this is the last index - the final expiration that we care about
		int barrier_not_hit = 1;
		if (up_or_down_ == 1 && spot >= barrier_) {
			barrier_not_hit = 0 ; // if barrier is hit, it will go to zero
		}
		if (up_or_down_ == 0 && spot <= barrier_) {
			barrier_not_hit = 0 ; //  if barrier is hit, it will go to zero
		}
		double payoff = barrier_not_hit * ((spot - strike_) * payoffType_);
		payAmounts_[idx] = payoff > 0.0 ? payoff : 0.0;
	}
	else {  // this is not the last index, check the exercise condition
		if (up_or_down_ == 1 && spot >= barrier_) {
			contValue = 0.0; // if barrier is hit, it will go to zero
		}
		if (up_or_down_ == 0 && spot <= barrier_) {
			contValue = 0.0; //  if barrier is hit, it will go to zero
		}
		//double intrinsicValue = 0.0; // Force the intrinsic value to be 0 because we do not allow for early exercise (This is a workaround so that we don't ever exercise it early)
		
		// I think there is no need for intrinsicValue
		
		payAmounts_[idx] = contValue > 0.0 ? contValue : 0.0; //ensure it is above 0
	}
}

END_NAMESPACE(orf)

#endif // ORF_BARRIERCALLPUT_HPP
