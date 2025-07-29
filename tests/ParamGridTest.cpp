#include <gtest/gtest.h>

#include "BlackScholes.h"
#include "MonteCarlo.h"
#include "Option.h"
#include "TestUtils.h"

struct GridParam {
  double S, K, T, r, sig, q;
  OptionType type;
};
class MCBSParamGrid : public ::testing::TestWithParam<GridParam> {};

TEST_P(MCBSParamGrid, MCWithin3SEofBS) {
  auto p = GetParam();
  Option opt = (p.type == OptionType::CALL)
                   ? Option::createCall(p.S, p.K, p.T, p.r, p.sig, p.q)
                   : Option::createPut(p.S, p.K, p.T, p.r, p.sig, p.q);

  MonteCarlo mc(opt, 150000, 321u);
  double priceMC = mc.calculatePrice();

  BlackScholes bs(opt);
  double priceBS = bs.calculatePrice();
  double se = mc.getStandardError();
  if (se < 1e-12) {
    EXPECT_NEAR(priceMC, priceBS, 1e-5) << "Deterministic case";
  } else {
    EXPECT_CLOSE_WITH_SE(priceMC, priceBS, se, 3.0, "MC vs BS price");
  }
}

INSTANTIATE_TEST_SUITE_P(
    ParamSweep, MCBSParamGrid,
    ::testing::Values(
        GridParam{80, 100, 0.25, 0.00, 0.10, 0.00, OptionType::CALL},
        GridParam{100, 100, 1.00, 0.05, 0.20, 0.00, OptionType::CALL},
        GridParam{120, 90, 2.00, 0.03, 0.40, 0.00, OptionType::PUT},
        GridParam{100, 110, 0.50, -0.01, 0.30, 0.02, OptionType::CALL},
        GridParam{50, 150, 1.50, 0.07, 0.15, 0.00, OptionType::PUT},
        GridParam{150, 50, 0.75, 0.02, 0.35, 0.05, OptionType::CALL}));
