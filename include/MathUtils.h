#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cmath>

namespace math {
constexpr double INV_SQRT_2PI = 0.39894228040143267794;
constexpr double INV_SQRT_2 = 0.70710678118654752440;

inline double norm_pdf(double x) {
  return INV_SQRT_2PI * std::exp(-0.5 * x * x);
}

inline double norm_cdf(double x) {
  return 0.5 * (1.0 + std::erfc(-x * INV_SQRT_2));
}
}  // namespace math

#endif  // MATHUTILS_H
