#include <orflib/math/matrix.hpp>
#include <orflib/methods/montecarlo/mcparams.hpp>
#include <orflib/methods/pde/pdeparams.hpp>
#include <xlw/xlw.h>

/** The tick and tock are unprintable characters; in Excel they look like a little square.
    We append the tick/tock at the end of every object name to trigger Excel recalculation
*/
enum XlTickTock
{
  XL_TICK = 164,
  XL_TOCK = 176
};

/** Adds the tick at the end of the string name
*/
std::string xlAddTick(std::string const & name);

/** Strips the tick from the string name.
    It returns the portion of the string up to the tick.
*/
std::string xlStripTick(std::string const & name);

/** Converts a row or column XlfOper range to an orf::Vector.
    It ignores any leading or trailing cells that are empty.
    The passed-in range must be 1xn or nx1.
*/
orf::Vector xlOperToVector(xlw::XlfOper xlVec);

/** Converts an orf::Matrix to an XlfOper
*/
xlw::XlfOper xlVectorToOper(orf::Vector const & vec, bool colMajor = true);

/** Converts a rectangular XlfOper range to an orf::Matrix.
    It ignores any leading or trailing rows or columns that are empty.
*/
orf::Matrix xlOperToMatrix(xlw::XlfOper xlMat);

/** Converts an orf::Matrix to an XlfOper
*/
xlw::XlfOper xlMatrixToOper(orf::Matrix const & mat);

/** Converts an Excel range with name-value pairs to an McParams structure.
    In the Excel range all names must be in first column and all values in the second.
*/
orf::McParams xlOperToMcParams(xlw::XlfOper xlRange);

/** Converts an Excel range with name-value pairs to an PdeParams structure.
    In the Excel range all names must be in first column and all values in the second.
*/
orf::PdeParams xlOperToPdeParams(xlw::XlfOper xlRange);