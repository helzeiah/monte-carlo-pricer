#include <gtest/gtest.h>

#include "BlackScholes.h"
#include "MonteCarlo.h"
#include "Option.h"

TEST(PricerInterface, CacheAndFlags) {
  Option opt = Option::createCall(100, 100, 1, 0.05, 0.2);
  BlackScholes bs(opt);
  EXPECT_FALSE(bs.isPriceCalculated());
  double p1 = bs.calculatePrice();
  EXPECT_TRUE(bs.isPriceCalculated());
  double p2 = bs.calculatePrice();
  EXPECT_DOUBLE_EQ(p1, p2);  // cached

  MonteCarlo mc(opt, 10000, 1u);
  mc.calculatePrice();
  EXPECT_THROW(mc.calculateVaR(-0.1), std::invalid_argument);
}