#ifndef TESTUTILS_H
#define TESTUTILS_H
#include <gtest/gtest.h>

inline void EXPECT_NEAR_REL(double actual, double expected, double relTol,
                            const char* msg) {
  double denom = std::max(std::abs(expected), 1.0);  // avoid 0 denom
  double absTol = std::max(relTol * denom, 1e-6);    // give tiny floor
  EXPECT_NEAR(actual, expected, absTol) << msg;
}

inline void EXPECT_CLOSE_WITH_SE(double actual, double expected, double se,
                                 double z = 3.0, const char* msg = "") {
  if (se == 0.0) {
    // Deterministic case: use a small absolute tolerance instead.
    const double absTol = std::max(1e-6, 1e-3 * std::abs(expected));
    EXPECT_NEAR(actual, expected, absTol)
        << msg << " (deterministic case, absTol=" << absTol << ")";
    return;
  }
  const double absTol = z * se;
  EXPECT_NEAR(actual, expected, absTol)
      << msg << " (absTol=" << absTol << ", se=" << se << ")";
}

#endif  // TESTUTILS_H
