/** 
@file  exception.hpp
@brief Definition of the orf::Exception class and the ORF_ASSERT macro
*/

#ifndef ORF_EXCEPTION_HPP
#define ORF_EXCEPTION_HPP

#include <exception>
#include <sstream>
#include <string>

namespace orf {

class Exception;

/** Streaming operator */
template <typename T> Exception & operator<<(Exception & ex, T const & msg);

/** The orflib Exception class; error messages can be streamed into it.
*/
class Exception : public std::exception 
{
public:
  /** initializing ctor */
  explicit Exception(std::string const & errmsg) : what_(errmsg) {}
  /** Returns the error message */
  virtual const char* what() const noexcept { return what_.c_str(); }
  /** dtor */
  virtual ~Exception() {}	

protected:
  mutable std::string what_;

  template <typename T>
  friend Exception & operator<<(Exception & ex, T const & msg);
};

// Implementation of the streaming operator
template <typename T>
Exception & operator<<(Exception & ex, T const & msg)
{
  std::ostringstream s;
  s << msg;
  ex.what_ += s.str();
  return ex;
}

/** @def    ORF_ASSERT
 *  @brief  Convenience macro for throwing an Exception if 'condition' evaluates to false;
 *          'errmsg' is the error message string.
 */

#define ORF_ASSERT(condition, errmsg) \
if (!(condition)) { \
  if ((std::string(errmsg)).empty()) \
  throw orf::Exception("error: " #condition); \
  else \
  throw orf::Exception(errmsg); \
}

} // namespace orf

#endif // ORF_EXCEPTION_HPP
