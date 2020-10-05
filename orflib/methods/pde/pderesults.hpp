/**
@file  pderesults.hpp
@brief Definition of the PdeResults and Pde1DResults classes
*/

#ifndef ORF_PDERESULTS_HPP
#define ORF_PDERESULTS_HPP

#include <orflib/methods/pde/pdegrid.hpp>
#include <vector>

BEGIN_NAMESPACE(orf)

class PdeResults
{
public:
  Vector prices;      // vector of size nLayers, with the prices at the current spots
  Vector times;       // vector of time nodes
  std::vector<GridAxis> gridAxes; // vector of size nAssets with the grid axes

  /** Computes the spot axis for the asset with index assetIdx */
  void getSpotAxis(size_t assetIdx, Vector& axis)
  {
    ORF_ASSERT(!gridAxes.empty(), "No grid axes info. in PDE results!");
    axis.resize(gridAxes[assetIdx].NX + 2);
    double xmin = gridAxes[assetIdx].Xmin;
    double dx = gridAxes[assetIdx].DX;
    for (size_t i = 0; i < axis.size(); ++i) {
      axis[i] = gridAxes[assetIdx].coordinateChange->fromDiffusedToReal(xmin + i * dx);
    }
  }

  size_t nAssets()
  {
    return gridAxes.size();
  }

protected:

  /** Computes the spot axes for each asset */
  void computeSpotAxes()
  {
    spotAxes.resize(nAssets());
    for (size_t i = 0; i < nAssets(); ++i) {
      getSpotAxis(i, spotAxes[i]);
    }
  }

  // state
  std::vector<Vector> spotAxes;  // a vector of spot axes vectors, could be jagged
};


class Pde1DResults : public PdeResults
{
public:
  std::vector<Matrix> values; // for each time a nSpots x nLayers matrix of values

  /** Returns the vector of times, the vector of spots and the matrix of values for
      a variable with index varIdx
  */
  void getValues(int varIdx, Vector& timeAxis, Vector& xAxis, Matrix& zValues)
  {
    timeAxis = times;
    getSpotAxis(0, xAxis);
    zValues.resize(times.size(), xAxis.size());

    for (size_t i = 0; i < times.size(); ++i) {
      for (size_t j = 0; j < xAxis.size(); ++j) {
        zValues(i, j) = values[i](j, varIdx);
      }
    }
  }
};

END_NAMESPACE(orf)


#endif  // #ifndef ORF_PDERESULTS_HPP
