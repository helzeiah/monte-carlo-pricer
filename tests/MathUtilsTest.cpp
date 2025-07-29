#include <gtest/gtest.h>

#include "MathUtils.h"

TEST(MathUtils, PdfIntegratesRoughlyToOne) {
  double h{0.001};
  double sum{0.0};
  for (double x{-6.0}; x <= 6.0; x += h) {
    sum += math::norm_pdf(x) * h;
  }
  EXPECT_NEAR(sum, 1.0, 1e-3);
}

TEST(MathUtils, CdfSymmetry) {
  EXPECT_NEAR(math::norm_cdf(0.0), 0.5, 1e-12);
  double x{1.234};
  EXPECT_NEAR(math::norm_cdf(-x), 1.0 - math::norm_cdf(x), 1e-12);
}
