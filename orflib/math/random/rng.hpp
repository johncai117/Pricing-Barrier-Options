/** 
@file  rng.hpp
@brief Type definitions for common random number generators
*/

#ifndef ORF_RNG_HPP
#define ORF_RNG_HPP

#include <orflib/math/random/normalrng.hpp>
#include <orflib/math/random/sobolurng.hpp>

BEGIN_NAMESPACE(orf)

/** Linear congruential */
using NormalRngMinStdRand = NormalRng<std::minstd_rand>;

/** Mersenne Twister */
using NormalRngMt19937 = NormalRng<std::mt19937>;

/** RanLux level 3 */
using NormalRngRanLux3 = NormalRng<std::ranlux24>;

/** RanLux level 4 */
using NormalRngRanLux4 = NormalRng<std::ranlux48>;

/** Sobol */
using NormalRngSobol = NormalRng<orf::SobolURng>;

END_NAMESPACE(orf)

#endif // ORF_RNG_HPP
