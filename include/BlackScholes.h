#ifndef BLACKSCHOLES_H
#define BLACKSCHOLES_H
#include "Pricer.h"

class BlackScholes : public Pricer {
public:
  explicit BlackScholes(const Option& option) : Pricer(option) {}

  double calculatePrice() const override;
  std::string getPricingMethod() const override { return "Black-Scholes"; }
  Greeks calculateGreeks() override;
  std::pair<double, double> getConfidenceInterval(double) override {
    throw std::runtime_error("Black-Scholes does not support confidence "
                             "intervals");
  }
  double getStandardError() override {
    throw std::runtime_error("Black-Scholes does not support standard error");
  }
  double calculateVaR(double) override {
    throw std::runtime_error("Black-Scholes does not support VaR");
  }
};

#endif //BLACKSCHOLES_H
