#include <gtest/gtest.h>

#include <cmath>

#include "BlackScholes.h"
#include "Option.h"

// reference values from standard BS calculator (no dividends)
TEST(BlackScholes, PriceCallNoDiv) {
  Option opt = Option::createCall(100.0, 100.0, 1.0, 0.05, 0.2);
  BlackScholes bs(opt);
  double price = bs.calculatePrice();
  EXPECT_NEAR(price, 10.4506, 1e-4);
}

TEST(BlackScholes, PricePutNoDiv) {
  Option opt = Option::createPut(100.0, 100.0, 1.0, 0.05, 0.2);
  BlackScholes bs(opt);
  double price = bs.calculatePrice();
  EXPECT_NEAR(price, 5.5735, 1e-4);
}

TEST(BlackScholes, GreeksCallNoDiv) {
  Option opt = Option::createCall(100, 100, 1.0, 0.05, 0.2);
  BlackScholes bs(opt);
  Greeks g = bs.calculateGreeks();
  EXPECT_NEAR(g.delta, 0.6368306512, 1e-6);
  EXPECT_NEAR(g.gamma, 0.0187620173, 1e-6);
  EXPECT_NEAR(g.vega, 37.52403465, 1e-4);    // decimal vols
  EXPECT_NEAR(g.theta, -6.414027546, 1e-3);  // per year
  EXPECT_NEAR(g.rho, 53.23248165, 1e-3);
}

TEST(BlackScholes, TZeroEqualsPayoff) {
  Option call = Option::createCall(120, 100, 0.0, 0.05, 0.2);
  BlackScholes bs(call);
  EXPECT_DOUBLE_EQ(bs.calculatePrice(),
                   call.calculatePayoff(call.getStockPrice()));
}

TEST(BlackScholes, SigmaZeroDeterministicForward) {
  Option put = Option::createPut(100, 110, 1.0, 0.05, 0.0);
  BlackScholes bs(put);
  double price = bs.calculatePrice();
  double ST = put.getStockPrice() *
              std::exp((put.getRiskFreeRate() - put.getDividendYield()) *
                       put.getTimeToMaturity());
  double payoff = put.calculatePayoff(ST);
  double disc = std::exp(-put.getRiskFreeRate() * put.getTimeToMaturity());
  EXPECT_NEAR(price, payoff * disc, 1e-12);
}