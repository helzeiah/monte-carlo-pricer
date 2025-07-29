#ifndef BLACKSCHOLES_H
#define BLACKSCHOLES_H
#include "Pricer.h"

/**
 * @brief Analytic Black-Scholes pricer for European options.
 *
 * Implements closed-form formulas for price and Greeks under
 * the Black-Scholes assumptions (no discrete dividends, lognormal stock).
 */
class BlackScholes : public Pricer {
 public:
  /**
   * @brief Constructs a Black-Scholes pricer for the given option.
   *
   * @param option the European option parameters.
   */
  explicit BlackScholes(const Option& option) : Pricer(option) {}

  /**
   * @brief Calculates the option price using the Black-Scholes formula.
   * @return the analytic price of the option.
   */
  double calculatePrice() const override;

  std::string getPricingMethod() const override { return "Black-Scholes"; }

  /**
   * @brief Computes option Greeks analytically (Δ, Γ, Θ, ν, ρ).
   * @return a Greeks struct populated with sensitivity values.
   */
  Greeks calculateGreeks() override;

  std::pair<double, double> getConfidenceInterval(double) override {
    throw std::runtime_error(
        "Black-Scholes does not support confidence "
        "intervals");
  }

  double getStandardError() override {
    throw std::runtime_error("Black-Scholes does not support standard error");
  }

  double calculateVaR(double) override {
    throw std::runtime_error("Black-Scholes does not support VaR");
  }
};

#endif  // BLACKSCHOLES_H
