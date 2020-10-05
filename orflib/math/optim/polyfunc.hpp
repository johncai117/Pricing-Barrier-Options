/**
@file   polyfunc.hpp
@brief  Definition of a Polynomial functor class
*/

#ifndef ORF_POLYFUNC_HPP
#define ORF_POLYFUNC_HPP

#include <orflib/defines.hpp>
#include <orflib/exception.hpp>
#include <orflib/math/matrix.hpp>

BEGIN_NAMESPACE(orf)

class Polynomial
{
public:
  Polynomial(orf::Vector const& coeffs) : coeffs_(coeffs) 
  {
    ORF_ASSERT(coeffs_.size() > 0, "Polynomial: empty vector of coefficients not allowed!")
  }

  double operator()(double x) 
  {
    double val = coeffs_[0];
    for (size_t i = 1; i < coeffs_.size(); ++i) {
      val += coeffs_[i] * std::pow(x, i);
    }
    return val;
  }

private:
  orf::Vector coeffs_;
};

END_NAMESPACE(orf)

#endif // ORF_POLYFUNC_HPP
