#ifndef PRICER_H
#define PRICER_H
#include <chrono>
#include <memory>

#include "Greeks.h"
#include "Option.h"

/**
 * @brief Abstract base class for option pricing engines.
 *
 * Provide a common interface for different pricing methods:
 * Monte Carlo, Black-Scholes, Binomial Trees, etc.
 */
class Pricer {
 protected:
  const Option& option;
  mutable bool priceCalculated{false};
  mutable double cachedPrice{0.0};
  mutable std::chrono::duration<double> lastRunDuration{0.0};

 public:
  explicit Pricer(const Option& option) : option(option) {}
  virtual ~Pricer() = default;

  /**
   * @brief Calculates the option price using this Pricer's method.
   *
   * @return The price of the option.
   */
  virtual double calculatePrice() const = 0;

  /**
   * @brief Gets the name of this pricing method.
   * @return the Method name (e.g., "Monte Carlo", "Black-Scholes")
   */
  virtual std::string getPricingMethod() const { return "Abstract Pricer"; }

  /**
   * @brief Calculates option Greeks (Delta, Gamma, Theta, Vega, Rho).
   * @return Greeks structure with sensitivity values
   */
  virtual Greeks calculateGreeks() = 0;

  /**
   * @brief Calculates Value at Risk at the given confidence level.
   * @param confidenceLevel Confidence level (default: 0.05 for 5% VaR)
   * @return VaR value
   * @throws std::runtime_error if this method doesn't support VaR
   */
  virtual double calculateVaR(double confidenceLevel = 0.05) {
    throw std::runtime_error(getPricingMethod() +
                             " does not support VaR calculation");
  }

  /**
   * @brief Gets the confidence interval for price estimate.
   * @param confidenceLevel Confidence level (default: 0.95 for 95% CI)
   * @return a pair of (lower bound, upper bound)
   * @throws std::runtime_error if this method doesn't support confidence
   * intervals
   */
  virtual std::pair<double, double> getConfidenceInterval(
      double confidenceLevel = 0.95) {
    throw std::runtime_error(getPricingMethod() +
                             " does not support confidence intervals");
  }

  /**
   * @brief Gets the standard error of price estimate.
   * @return the standard error
   * @throws std::runtime_error if method doesn't support standard error
   */
  virtual double getStandardError() {
    throw std::runtime_error(getPricingMethod() +
                             " does not support standard error");
  }

  /**
   * @brief Gets the execution time of the last calculation.
   * @return the time in seconds
   */
  virtual double getLastCalculationTime() const {
    return lastRunDuration.count();
  }

  /**
   * @brief Checks if price has been calculated.
   * @return true if price is calculated, false otherwise
   */
  virtual bool isPriceCalculated() const { return priceCalculated; }

  /**
   * @brief Gets convergence information (method-specific).
   * @return Convergence description
   */
  virtual std::string getConvergenceInfo() const {
    return "No convergence information available for Generic Pricer";
  }

  /**
   * @brief Gets the option being priced.
   * @return a reference to the option object
   */
  const Option& getOption() const { return option; }

  /**
   * @brief Gets the calculated price (cached value).
   * @return the option price
   * @throws std::runtime_error if price has not been calculated
   */
  double getPrice() const {
    if (!priceCalculated) {
      throw std::runtime_error("Price not calculated");
    }
    return cachedPrice;
  }
};

#endif  // PRICER_H
