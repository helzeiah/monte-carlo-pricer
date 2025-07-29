#include <gtest/gtest.h>

#include "BlackScholes.h"
#include "MonteCarlo.h"
#include "Option.h"

TEST(Caching, BSPriceCached) {
  Option opt = Option::createCall(100, 100, 1, 0.05, 0.2);
  BlackScholes bs(opt);
  double p1 = bs.calculatePrice();
  double t1 = bs.getLastCalculationTime();
  double p2 = bs.calculatePrice();
  double t2 = bs.getLastCalculationTime();
  EXPECT_DOUBLE_EQ(p1, p2);
  EXPECT_LE(t2, t1);  // second call should be fast / zero
}

TEST(Caching, MCRunMoreSimulationsUpdates) {
  Option opt = Option::createCall(100, 100, 1, 0.05, 0.2);
  MonteCarlo mc(opt, 20000, 11u);
  double p1 = mc.calculatePrice();
  double se1 = mc.getStandardError();
  mc.runMoreSimulations(100000);
  double p2 = mc.getPrice();
  double se2 = mc.getStandardError();
  EXPECT_NEAR(p1, p2, 3 * se2);  // they should be close
  EXPECT_LT(se2, se1);
}