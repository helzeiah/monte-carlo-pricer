#include <gtest/gtest.h>

#include "BlackScholes.h"
#include "ImpliedVol.h"
#include "Option.h"

TEST(ImpliedVol, RecoversSigmaFromBSPrice) {
  Option opt = Option::createCall(100, 120, 0.75, 0.02, 0.35, 0.01);
  BlackScholes bs(opt);
  double target = bs.calculatePrice();

  double sigma_est = impliedVolBS(opt, target, 1e-8, 100);
  EXPECT_NEAR(sigma_est, 0.35, 1e-4);
}