#include <xlorflib/xlutils.hpp>
#include <orflib/exception.hpp>
#include <orflib/utils.hpp>

using namespace xlw;
using namespace orf;

std::string xlAddTick(std::string const & name)
{
  return name + (char)XL_TICK;
}

std::string xlStripTick(std::string const & name)
{
  char chtick = (char)XL_TICK;
  size_t loc = name.find(chtick, 0);
  std::string strippedName;
  std::string finaltag;
  if (loc != std::string::npos)
    strippedName = name.substr(0, loc);
  else
    strippedName = name;
  return strippedName;
}

/** Utility function that scans a rectangular XlfOper range, and selects the rectangular subrange
    that does not contain leading or trailing empty rows and columns.
    It computes the start and end row and column indices of the subrange.
    It returns true if the subrange is smaller than the range.
*/
bool xlScanForBlanks(XlfOper xlRange,
                     RW & startRowIdx,
                     RW & endRowIdx,
                     COL & startColIdx,
                     COL & endColIdx)
{
  RW  const nr = xlRange.rows();
  COL const nc = xlRange.columns();
  startRowIdx = 0;
  endRowIdx = nr - 1;
  startColIdx = 0;
  endColIdx = nc - 1;
  bool ret(false);

  // column scan left-to-right (from first to last)
  for (COL j = 0; j <= endColIdx; ++j) {
    RW i = 0;
    while (i <= endRowIdx) {
      XlfOper xlval = xlRange(i, j);
      if (xlval.IsMissing() || xlval.IsNil() || xlval.IsError())
        ++i;
      else
        break;
    }
    if (i == endRowIdx + 1) {
      startColIdx++;  // detected a column that is all emtpy
      ret = true;
    }
    else
      break;          // now leading columns that are empty

  }
  // column scan right-to-left (from last to first)
  COL endc = endColIdx;
  for (COL j = endc; j >= startColIdx; --j) {
    RW i = 0;
    while (i <= endRowIdx) {
      XlfOper xlval = xlRange(i, j);
      if (xlval.IsMissing() || xlval.IsNil() || xlval.IsError())
        ++i;
      else
        break;
    }
    if (i == endRowIdx + 1) {
      endColIdx--;  // detected a column that is all empty
      ret = true;
    }
    else
      break;        // no trailing columns that are empty
  }
  // row scan top-to-bottom (from first to last)
  for (RW i = 0; i <= endRowIdx; ++i) {
    COL j = startColIdx;
    while (j <= endColIdx) {
      XlfOper xlval = xlRange(i, j);
      if (xlval.IsMissing() || xlval.IsNil() || xlval.IsError())
        ++j;
      else
        break;
    }
    if (j == endColIdx + 1) {
      startRowIdx++;  // detected a row that is all empty
      ret = true;
    }
    else
      break;          // no leading raws that are empty
  }
  // row scan bottom-to-top (from last to first)
  RW endr = endRowIdx;
  for (RW i = endr; i >= startRowIdx; --i) {
    COL j = startColIdx;
    while (j <= endColIdx) {
      XlfOper xlval = xlRange(i, j);
      if (xlval.IsMissing() || xlval.IsNil() || xlval.IsError())
        ++j;
      else
        break;
    }
    if (j == endColIdx + 1) {
      endRowIdx--;  // detected a column that is all empty
      ret = true;
    }
    else
      break;
  }

  return ret;
}

/** Converts a row or column XlfOper range to an orf::Vector.
    It ignores any leading or trailing cells are empty.
    The passed-in range must be 1xn or nx1.
*/
Vector xlOperToVector(xlw::XlfOper xlRange)
{
  // handle the single cell case
  if (!xlRange.IsMulti()) {
    if (xlRange.IsMissing() || xlRange.IsNil() || xlRange.IsError())
      ORF_ASSERT(0, "xlOperToVector: no values passed in");
    Vector vec(1);
    vec(0) = xlRange.AsDouble("xlOperToVector: failed to convert cell to double");
    return vec;
  }

  RW  nr = xlRange.rows();
  COL nc = xlRange.columns();
  ORF_ASSERT(nr == 1 || nc == 1, "xlOperToVec: the passed-in range is not 1-D");
  RW startRowIdx = 0;
  RW endRowIdx = nr - 1;
  COL startColIdx = 0;
  COL endColIdx = nc - 1;

  // scan the range for blanks/missing/error values
  bool ret = xlScanForBlanks(xlRange, startRowIdx, endRowIdx, startColIdx, endColIdx);

  size_t nrow = endRowIdx - startRowIdx + 1;
  size_t ncol = endColIdx - startColIdx + 1;
  size_t n = nrow * ncol;
  ORF_ASSERT(n > 0, "xlOperToVector: no values passed in");
  Vector vec(n);

  Vector::iterator vit = vec.begin();  // get an iterator into the vector
  for (RW i = startRowIdx; i <= endRowIdx; ++i) {
    for (COL j = startColIdx; j <= endColIdx; ++j) {
      *vit = xlRange(i, j).AsDouble("xlOperToVector: failed to convert cell to double");
      ++vit;
    }
  }
  return vec;
}

/** Converts an orf::Vector to an xloper
*/
XlfOper xlVectorToOper(Vector const& vec, bool colMajor)
{
  RW nr = colMajor ? (RW)vec.size() : 1;
  COL nc = colMajor ? 1 : (RW)vec.size();
  XlfOper xlRange(nr, nc);
  for (RW i = 0; i < nr; ++i) {
    for (COL j = 0; j < nc; ++j) {
      xlRange(i, j) = vec(i * colMajor + j * !colMajor);
    }
  }
  return xlRange;
}


/** Converts a rectangular xloper range to an orf::Matrix.
    It ignores any leading or trailing rows or columns that are empty.
*/
Matrix xlOperToMatrix(XlfOper xlRange)
{
  // handle the single cell case
  if (!xlRange.IsMulti()) {
    if (xlRange.IsMissing() || xlRange.IsNil() || xlRange.IsError())
      ORF_ASSERT(0, "xlOperToMatrix: no values passed in");
    Matrix mat(1, 1);
    mat(0, 0) = xlRange.AsDouble("xlOperToMatrix: failed to convert cell to double");
    return mat;
  }

  RW  nr = xlRange.rows();
  COL nc = xlRange.columns();
  RW startRowIdx = 0;
  RW endRowIdx = nr - 1;
  COL startColIdx = 0;
  COL endColIdx = nc - 1;

  // scan the range for blanks/missing/error values
  bool ret = xlScanForBlanks(xlRange, startRowIdx, endRowIdx, startColIdx, endColIdx);

  size_t nrow = endRowIdx - startRowIdx + 1;
  size_t ncol = endColIdx - startColIdx + 1;
  ORF_ASSERT(nrow * ncol > 0, "xlOperToMatrix: no values passed in");
  Matrix mat(nrow, ncol);

  size_t ii(0), jj(0);       // initialize row and column indices of matrix
  for (RW i = startRowIdx; i <= endRowIdx; ++i) {
    jj = 0;                  // reset column index of matrix
    for (COL j = startColIdx; j <= endColIdx; ++j) {
      mat(ii, jj) = xlRange(i, j).AsDouble("xlOperToMatrix: failed to convert cell to double");
      ++jj;
    }
    ++ii;
  }

  return mat;
}


/** Converts an orf::Matrix to an xloper
*/
XlfOper xlMatrixToOper(Matrix const & mat)
{
  RW nr = (RW)mat.n_rows;
  COL nc = (COL)mat.n_cols;
  XlfOper xlRange(nr, nc);
  for (RW i = 0; i < nr; ++i) {
    for (COL j = 0; j < nc; ++j) {
      xlRange.SetElement(i, j, mat(i, j));
    }
  }
  return xlRange;
}

/** Converts an Excel range with name-value pairs to an McParams structure.
    In the Excel range all names must be in first column and all values in the second.
*/
orf::McParams xlOperToMcParams(xlw::XlfOper xlRange)
{
  McParams mcparams;
  if (xlRange.IsMissing() || xlRange.IsNil())
    return mcparams;

  RW  nr = xlRange.rows();
  COL nc = xlRange.columns();

  ORF_ASSERT(nr > 0, "xlOperToMcParams: the input range is empty!");
  ORF_ASSERT(nc == 2, "xlOperToMcParams: the input range must have two columns!");

  // row scan
  for (RW i = 0; i < nr; ++i) {
    std::string paramname = xlRange(i, 0).AsString();
    paramname = orf::trim(paramname);
    std::transform(paramname.begin(), paramname.end(), paramname.begin(), ::toupper);

    if (paramname == "URNGTYPE") {
      std::string paramvalue = xlRange(i, 1).AsString();
      paramvalue = orf::trim(paramvalue);
      std::transform(paramvalue.begin(), paramvalue.end(), paramvalue.begin(), ::toupper);

      if (paramvalue == "MINSTDRAND")
        mcparams.urngType = McParams::UrngType::MINSTDRAND;
      else if (paramvalue == "MT19937")
        mcparams.urngType = McParams::UrngType::MT19937;
      else if (paramvalue == "RANLUX3")
        mcparams.urngType = McParams::UrngType::RANLUX3;
      else if (paramvalue == "RANLUX4")
        mcparams.urngType = McParams::UrngType::RANLUX4;
      else if (paramvalue == "SOBOL")
        mcparams.urngType = McParams::UrngType::SOBOL;
      else
        ORF_ASSERT(0, "xlOperToMcParams: invalid value for McParam " + paramname + "!");
    }
    else  if (paramname == "PATHGENTYPE") {
      std::string paramvalue = xlRange(i, 1).AsString();
      paramvalue = orf::trim(paramvalue);
      std::transform(paramvalue.begin(), paramvalue.end(), paramvalue.begin(), ::toupper);
      if (paramvalue == "EULER")
        mcparams.pathGenType = McParams::PathGenType::EULER;
      else
        ORF_ASSERT(0, "xlOperToMcParams: invalid value for McParam " + paramname + "!");
    }
    else
      ORF_ASSERT(0, "xlOperToMcParams: unknown McParam " + paramname + "!");
  } // next row in the range
  return mcparams;
}

/** Converts an Excel range with name-value pairs to an PdeParams structure.
In the Excel range all names must be in first column and all values in the second.
*/
orf::PdeParams xlOperToPdeParams(xlw::XlfOper xlRange)
{
  RW  nr = xlRange.rows();
  COL nc = xlRange.columns();

  ORF_ASSERT(nr > 0, "xlOperToPdeParams: the input range is empty!");
  ORF_ASSERT(nc == 2, "xlOperToPdeParams: the input range must have two columns!");
  PdeParams pdeparams;
  // row scan
  for (RW i = 0; i < nr; ++i) {
    std::string paramname = xlRange(i, 0).AsString();
    paramname = orf::trim(paramname);
    std::transform(paramname.begin(), paramname.end(), paramname.begin(), ::toupper);

    if (paramname == "NTIMESTEPS") {
      int paramvalue = xlRange(i, 1).AsInt();
      ORF_ASSERT(paramvalue > 0, "xlOperToPdeParams: the number of time steps must be positive!");
      pdeparams.nTimeSteps = paramvalue;
    }
    else if (paramname == "NSPOTNODES" || paramname == "NSPOTNODES1") {
      int paramvalue = xlRange(i, 1).AsInt();
      ORF_ASSERT(paramvalue > 0, "xlOperToPdeParams: the number of spot nodes must be positive!");
      pdeparams.nSpotNodes[0] = paramvalue;
    }
    else if (paramname == "NSTDDEVS" || paramname == "NSTDDEVS1") {
      double paramvalue = xlRange(i, 1).AsDouble();
      ORF_ASSERT(paramvalue > 0, "xlOperToPdeParams: the number of standard deviations must be positive!");
      pdeparams.nStdDevs[0] = paramvalue;
    }
    else if (paramname == "THETA") {
      double paramvalue = xlRange(i, 1).AsDouble();
      ORF_ASSERT(paramvalue >= 0.0 && paramvalue <= 1.0, "xlOperToPdeParams: Theta must be between 0 and 1!");
      pdeparams.theta = paramvalue;
    }
    else
      ORF_ASSERT(0, "xlOperToPdeParams: unknown PdeParam " + paramname + "!");
  } // next row in the range
  return pdeparams;
}
