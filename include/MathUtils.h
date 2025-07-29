#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cmath>

namespace math {
constexpr double INV_SQRT_2PI = 0.39894228040143267794;
constexpr double INV_SQRT_2 = 0.70710678118654752440;

/**
 * @brief Probability density function of the standard normal distribution.
 * @param x the point at which to evaluate the PDF.
 * @return the value of the PDF at x.
 */
inline double norm_pdf(double x) {
  return INV_SQRT_2PI * std::exp(-0.5 * x * x);
}

/**
 * @brief Cumulative distribution function of the standard normal distribution.
 * @param x the point at which to evaluate the CDF.
 * @return the value of the CDF at x.
 */
inline double norm_cdf(double x) { return 0.5 * std::erfc(-x * INV_SQRT_2); }
}  // namespace math

#endif  // MATHUTILS_H
