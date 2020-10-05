/**
@file  pdeparams.hpp
@brief Definition of the PdeParams class
*/

#ifndef ORF_PDEPARAMS_HPP
#define ORF_PDEPARAMS_HPP

#include <vector>

BEGIN_NAMESPACE(orf)

struct PdeParams
{
public:
  size_t nTimeSteps;
  std::vector<size_t> nSpotNodes; // spot nodes for each dimension
  std::vector<double> nStdDevs;   // num. standard deviations for each dimension
  double theta;

  /** Default ctor */
  PdeParams(size_t n = 1) : nTimeSteps(1), nSpotNodes(n, 10), nStdDevs(n, 4.0), theta(0.0) {};
};



END_NAMESPACE(orf)


#endif  // #ifndef ORF_PDEPARAMS_HPP
