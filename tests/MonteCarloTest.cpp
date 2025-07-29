#include <gtest/gtest.h>

#include "BlackScholes.h"
#include "MonteCarlo.h"
#include "TestUtils.h"

TEST(MonteCarlo, PriceCloseToBSWithin3SE) {
  Option opt = Option::createCall(100, 100, 1, 0.05, 0.2);
  MonteCarlo mc(opt, 200000, 42u);
  double mcPrice = mc.calculatePrice();
  double se = mc.getStandardError();

  BlackScholes bs(opt);
  double bsPrice = bs.calculatePrice();

  double relTol = (3.0 * se) / std::max(1.0, std::fabs(bsPrice));
  EXPECT_NEAR_REL(mcPrice, bsPrice, relTol, "MC vs BS price");
}

TEST(MonteCarlo, StandardErrorShrinks) {
  Option opt = Option::createCall(100, 100, 1, 0.05, 0.2);
  MonteCarlo mc(opt, 30000, 7u);
  mc.calculatePrice();
  double se1 = mc.getStandardError();
  mc.runMoreSimulations(120000);
  double se2 = mc.getStandardError();
  EXPECT_LT(se2, se1);
}

TEST(MonteCarlo, GreeksRoughMatchBS) {
  Option opt = Option::createCall(100, 100, 1, 0.05, 0.2);
  MonteCarlo mc(opt, 400000, 1337u);
  Greeks gMC = mc.calculateGreeks();

  BlackScholes bs(opt);
  Greeks gBS = bs.calculateGreeks();

  EXPECT_NEAR_REL(gMC.delta, gBS.delta, 1e-2, "delta");
  EXPECT_NEAR_REL(gMC.gamma, gBS.gamma, 3e-2, "gamma");
  EXPECT_NEAR_REL(gMC.vega, gBS.vega, 7e-2, "vega");
  EXPECT_NEAR_REL(gMC.theta, gBS.theta, 3e-1, "theta");
  EXPECT_NEAR_REL(gMC.rho, gBS.rho, 7e-2, "rho");
}

TEST(MonteCarlo, ConfidenceIntervalContainsPrice) {
  Option opt = Option::createCall(100, 100, 1, 0.05, 0.2);
  MonteCarlo mc(opt, 100000, 99u);
  double price = mc.calculatePrice();
  auto [lo, hi] = mc.getConfidenceInterval(0.95);
  EXPECT_LT(lo, price);
  EXPECT_GT(hi, price);
}

TEST(MonteCarlo, ConstructorThrowsOnZeroNsims) {
  Option opt = Option::createCall(100, 100, 1, 0.05, 0.2);
  EXPECT_THROW(MonteCarlo mc(opt, 0), std::invalid_argument);
}

TEST(MonteCarlo, CIRejectsBadAlpha) {
  Option opt = Option::createCall(100, 100, 1, 0.05, 0.2);
  MonteCarlo mc(opt, 10000, 1u);
  mc.calculatePrice();
  EXPECT_THROW(mc.getConfidenceInterval(-0.1), std::invalid_argument);
  EXPECT_THROW(mc.getConfidenceInterval(1.1), std::invalid_argument);
}