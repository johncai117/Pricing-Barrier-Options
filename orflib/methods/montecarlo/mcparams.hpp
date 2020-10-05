/**
@file  mcparams.hpp
@brief Monte Carlo parameters
*/

#ifndef ORF_MCPARAMS_HPP
#define ORF_MCPARAMS_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>

BEGIN_NAMESPACE(orf)


/** Monte Carlo parameters
*/
struct McParams
{
  /** The known URNG types */
  enum class UrngType
  {
    MINSTDRAND,
    MT19937,
    RANLUX3,
    RANLUX4,
    SOBOL
  };

  /** The known path generator types */
  enum class PathGenType
  {
    EULER
  };


  /** Default ctor */
  McParams(UrngType u = UrngType::MT19937, PathGenType p = PathGenType::EULER);

  // state
  UrngType urngType;
  PathGenType pathGenType;
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
McParams::McParams(UrngType u, PathGenType p)
: urngType(u), pathGenType(p)
{}

END_NAMESPACE(orf)

#endif // ORF_MCPARAMS_HPP
