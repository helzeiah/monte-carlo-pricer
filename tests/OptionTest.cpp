#include <gtest/gtest.h>

#include "Option.h"

TEST(Option, PayoffCallPut) {
  Option c = Option::createCall(100, 90, 1, 0.05, 0.2);
  Option p = Option::createPut(100, 90, 1, 0.05, 0.2);
  EXPECT_DOUBLE_EQ(c.calculatePayoff(120), 30.0);
  EXPECT_DOUBLE_EQ(c.calculatePayoff(80), 0.0);
  EXPECT_DOUBLE_EQ(p.calculatePayoff(80), 10.0);
  EXPECT_DOUBLE_EQ(p.calculatePayoff(120), 0.0);
}

TEST(Option, ThrowsOnBadParams) {
  EXPECT_THROW(Option(OptionType::CALL, -1, 100, 1, 0.01, 0.2),
               std::invalid_argument);
  EXPECT_THROW(Option(OptionType::CALL, 100, 0, 1, 0.01, 0.2),
               std::invalid_argument);
  EXPECT_THROW(Option(OptionType::CALL, 100, 100, -1, 0.01, 0.2),
               std::invalid_argument);
  EXPECT_THROW(Option(OptionType::CALL, 100, 100, 1, 0.01, -0.2),
               std::invalid_argument);
  EXPECT_THROW(Option(OptionType::CALL, 100, 100, 1, 0.01, 0.2, -0.1),
               std::invalid_argument);
}
