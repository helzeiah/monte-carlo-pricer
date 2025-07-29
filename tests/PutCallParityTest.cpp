#include <gtest/gtest.h>

#include <cmath>

#include "BlackScholes.h"
#include "MonteCarlo.h"
#include "Option.h"

static double parityGapBS(const Option& c, const Option& p) {
  BlackScholes bc(c), bp(p);
  double C = bc.calculatePrice();
  double P = bp.calculatePrice();
  double Sdisc = c.getStockPrice() *
                 std::exp(-c.getDividendYield() * c.getTimeToMaturity());
  double Kdisc = c.getStrikePrice() *
                 std::exp(-c.getRiskFreeRate() * c.getTimeToMaturity());
  return std::fabs((C - P) - (Sdisc - Kdisc));
}

static double parityGapMC(const Option& c, const Option& p,
                          unsigned long n = 1000000, unsigned seed = 123) {
  MonteCarlo mcC(c, n, seed);
  MonteCarlo mcP(p, n, seed);
  double C = mcC.calculatePrice();
  double P = mcP.calculatePrice();
  double Sdisc = c.getStockPrice() *
                 std::exp(-c.getDividendYield() * c.getTimeToMaturity());
  double Kdisc = c.getStrikePrice() *
                 std::exp(-c.getRiskFreeRate() * c.getTimeToMaturity());
  return std::fabs((C - P) - (Sdisc - Kdisc));
}

TEST(PutCallParity, BSExactWithinTol) {
  Option call = Option::createCall(100, 105, 1.0, 0.03, 0.25, 0.02);
  Option put = Option::createPut(100, 105, 1.0, 0.03, 0.25, 0.02);
  EXPECT_NEAR(parityGapBS(call, put), 0.0, 1e-10);
}

TEST(PutCallParity, MCApproxHolds) {
  Option call = Option::createCall(100, 105, 1.0, 0.03, 0.25, 0.02);
  Option put = Option::createPut(100, 105, 1.0, 0.03, 0.25, 0.02);
  double gap = parityGapMC(call, put);
  EXPECT_LT(gap, 5e-2);  // loose; stochastic
}