/**
@file  product.hpp
@brief Base class for all financial products.
*/

#ifndef ORF_PRODUCT_HPP
#define ORF_PRODUCT_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>
#include <orflib/sptr.hpp>
#include <orflib/math/matrix.hpp>

BEGIN_NAMESPACE(orf)

/** The abstract base class for all financial products.
    It must be inherited by specific product payoffs.
*/
class Product
{
public:
  /** Initializing ctor */
  explicit Product(std::string const& payccy = "USD");

  /** Dtor */
  virtual ~Product() {}

  /** Returns the fixing (observation) times */
  Vector const& fixTimes() const;

  /** Returns the payment times */
  Vector const& payTimes() const;

  /** Returns the payment amounts */
  Vector const& payAmounts() const;

  /** Returns the number of assets this product depends on */
  virtual size_t nAssets() const = 0;

  /** Evaluates the product given the passed-in path
      The "pricePath" matrix must have as many rows as the number of fixing times
  */
  virtual void eval(Matrix const& pricePath) = 0;

  /** Evaluates the product at fixing time index idx, for a vector of current spots,
      and a given continuation value.
      Useful for PDE pricing of products with early exercise features.
  */
  virtual void eval(size_t idx, Vector const& spots, double contValue) = 0;

  /** Sets up the time steps, to be used in a numerical method.
  The timesteps are returned in the std::vector<double> timesteps,
  and for each timestep, the corresponding index in the fixingTimes() array
  is in the std::vector<long> stepindex array.
  If stepindex[i] returns -1, this means that timesteps[i] does not correspond
  to a fixing time.
  */
  virtual void timeSteps(size_t nsteps,
                         std::vector<double>& timesteps,
                         std::vector<ptrdiff_t>& stepindex) const;

protected:
  std::string payccy_;
  Vector fixTimes_;       // the fixing (observation) times
  Vector payTimes_;       // the payment times
  Vector payAmounts_;     // the payment times
};

/** Smart pointer to Product */
using SPtrProduct = std::shared_ptr<Product>;

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
Product::Product(std::string const& payccy)
: payccy_(payccy)
{}

inline
Vector const& Product::fixTimes() const
{
  return fixTimes_;
}

inline
Vector const& Product::payTimes() const
{
  return payTimes_;
}

inline
Vector const& Product::payAmounts() const
{
  return payAmounts_;
}

inline
void Product::timeSteps(size_t nsteps,
                        std::vector<double>& timesteps,
                        std::vector<ptrdiff_t>& stepindex) const
{
  timesteps.clear();
  stepindex.clear();

  // first put all the fixing times into a temp array, starting with t = 0
  std::vector<double> tstemp(1, 0.0);
  // put the indices also in a temp array
  std::vector<ptrdiff_t> idxtemp(1, -1);
  for (size_t i = 0; i < fixTimes().size(); ++i) {
    tstemp.push_back(fixTimes()[i]);
    idxtemp.push_back(i);
  }
  // if t = 0 is a fixing time, remove the extra t = 0 point
  if (tstemp[0] == tstemp[1]) {
    tstemp.erase(tstemp.begin());
    idxtemp.erase(idxtemp.begin());
  }

  // NOTE: here we can put other time steps such as ex-div dates for discrete divs.

  // compute the timestep size
  double maxTime = tstemp[tstemp.size() - 1];
  double maxdt = maxTime / std::max(nsteps, size_t(1));

  for (size_t i = 0; i < tstemp.size() - 1; ++i) {
    timesteps.push_back(tstemp[i]);           // add the time step
    stepindex.push_back(idxtemp[i]);          // add the index
    double dt = tstemp[i + 1] - tstemp[i];
    if (dt - maxdt > 1.0e-8) {                // insert more steps in between
      size_t n = size_t(dt / maxdt);
      dt /= n;
      double T1 = tstemp[i];
      for (size_t j = 1; j < n; ++j) {
        timesteps.push_back(T1 + j * dt);
        stepindex.push_back(-1);  // this is not a product event
      }
    }
  }
  timesteps.push_back(tstemp.back());
  stepindex.push_back(idxtemp.back());
}

END_NAMESPACE(orf)

#endif // ORF_PRODUCT_HPP
