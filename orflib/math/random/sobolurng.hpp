/**
*   @file  sobolurng.hpp
*   @brief Generator of Sobol sequences
*/

#ifndef ORF_SOBOLURNG_HPP
#define ORF_SOBOLURNG_HPP


#include <orflib/defines.hpp>
#include <orflib/exception.hpp>
#include <vector>


BEGIN_NAMESPACE(orf)

/** Generator of a Sobol low discrepancy sequence.
*/
class SobolURng
{

public:

  /** Required for compatibility with std generators */
  using result_type = double;

  /** Initializing ctor */
  explicit SobolURng(size_t dimension);

  /** Default ctor */
  SobolURng() : SobolURng(1) {};

  /** Dtor */
  ~SobolURng();

  /** Returns the dimension of the generator */
  size_t dim() const;

  /** Returns a batch of random deviates
      CAUTION: it requires end - begin to be a divisor of dimension() 
   */
  template <typename ITER>
  void next(ITER begin, ITER end);

  /** Returns the next Sobol number.
      This method is provided to make SobolURng compatible with the URNGs in std.
      It should be called exactly dim() times to get one Sobol point (vector).
  */
  double operator()();

  double min() { return 2.0e-16; }

  double max() { return 1.0; }

  /** A Sobol generator cannot be seeded like a pseudorandom generator.
      This is for compatibility with URNGs.
      */
  void seed(unsigned long x0 = 0) {};

protected:

  /** Method with the initializing logic */
  void init(size_t dimension);

private:
  // no copy ctor or assignment op 
  SobolURng(SobolURng const&) = delete;
  SobolURng& operator=(SobolURng const&) = delete;

  // state
  enum { MAXBIT = 30 };

  size_t  dim_;               // the number of dimensions
  std::vector<double> point_; // the Sobol point in dim_ dimensions
  size_t curridx_;            // the current index in the point_ vector

  //	long*	pol;
  std::vector<long> otpol;  // vector with the polynomial encodings
  std::vector<long> deg;    // vector with the corresponding polynomial degrees
  long*	iv;                 // MAXBIT * ndim matrix of integer values
  long	in;
  std::vector<long>	ix;     // the vector of components
  std::vector<long*>	iu;   // allows 2D access into iv
  double	fac;              // the 1/2^MAXBIT normalizing factor

  // helper methods
  /** Initializes the primitive polynomials */
  long polyInit(long dimension);
  /** Initializes direction numbers */
  long ivInit(long dimension);
  /** creates the next sequence point */
  void nextPoint();
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
SobolURng::SobolURng(size_t dimension)
: dim_(dimension), point_(dimension), curridx_(dimension),
otpol(dimension), deg(dimension), ix(dimension), iu(MAXBIT), in(0), fac(1.00 / (1L << MAXBIT))
{
  ORF_ASSERT(dimension > 0, "the dimension must be positive!");
  init(dimension);
}

inline
SobolURng::~SobolURng()
{
  free(iv);
  iv = 0;
}

inline
size_t SobolURng::dim() const
{
  return dim_;
}

inline
void SobolURng::nextPoint()
{
  size_t im = in++;
  size_t j;
  for (j = 0; j < MAXBIT; ++j) {
    if (!(im & 1)) break;
    im >>= 1;
  }
  im = j * dim_;

  for (size_t k = 0; k < dim_; ++k) {
    ix[k] ^= iv[im + k];
    point_[k] = ix[k] * fac;
  }
}

template <typename ITER>
inline
void SobolURng::next(ITER begin, ITER end)
{
  size_t ncomp = 0;
  for (auto it = begin; it != end; ++it)
    ncomp++;
  ORF_ASSERT(ncomp <= dim_, "SobolURng::next(), size of range to fill is too large");
  ORF_ASSERT( dim_ % ncomp == 0, "SobolURng::next(), size of range to fill is not a divisor of dim")

  if (curridx_ == dim_) {  // generate a new point
    nextPoint();
    curridx_ = 0;
  }
  for (auto it = begin; it != end; ++it) {
    *it = point_[curridx_++];
  }
}

inline
double SobolURng::operator()()
{
  if (curridx_ == dim_) {  // generate a new point
    next(point_.begin(), point_.end());
    curridx_ = 0;
  }
  return point_[curridx_++];
}

END_NAMESPACE(orf)

#endif // ORF_SOBOLURNG_HPP
