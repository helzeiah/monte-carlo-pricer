#ifndef MONTECARLO_H
#define MONTECARLO_H
#include <random>
#include <vector>
#include <chrono>
#include <utility>
#include "Option.h"

/**
 * @brief Monte Carlo Option pricer using geometric Brownian motion.
 *
 * Implements industry-standard Monte Carlo simulation for European option
 * pricing. Uses geometric Brownian motion for stock price evolution and
 * includes statistical analysis capabilities for risk management applications.
 */
class MonteCarlo {
  Option option;
  unsigned long numSimulations{};

  // stored simulation results
  std::vector<double> payoffs{};

  mutable std::default_random_engine randomEngine{};
  mutable std::normal_distribution<double> standardNormal{};

  std::chrono::duration<double> lastRunDuration{};
  bool priceCalculated{};
  double cachedPrice{};

 public:
  /**
   * @brief Constructs a Monte Carlo pricer for the given option.
   *
   * @param option The European option to price
   * @param numSimulations The Number of simulation paths to run (default
   * 100,000)
   */
  MonteCarlo(const Option& option, unsigned long numSimulations = 100000);

  /**
   * @brief Constructs a Monte Carlo pricer with the specified random seed.
   *
   * @param option The European option to price
   * @param numSimulations The Number of simulation paths to run (default
   * 100,000)
   * @param seed The random seed for reproducible results and testing.
   */
  MonteCarlo(const Option& option, unsigned long numSimulations,
             unsigned int seed);

  /**
   * @brief Gets the option being priced by this Monte Carlo object.
   *
   * @return A copy of the option object being priced.
   */
  Option getOption() const;

  /**
   * @brief Gets the number of simulations used in this Monte Carlo object.
   *
   * @return The number of simulation paths used.
   */
  unsigned long getNumSimulations() const;

  /**
   * @brief Calculates the option price using Monte Carlo simulation.
   *
   * Runs the specified number of simulations, calculates payoffs,
   * and returns the discounted expected value.
   *
   * @return The estimated price of the option.
   */
  double calculatePrice();

  /**
   * @brief Calculates a 95% confidence interval for the price estimate.
   *
   * @return A Pair containing (lower bound, upper bound) of the interval.
   */
  std::pair<double, double> getConfidenceInterval() const;

  /**
   * @brief Runs additional simulations specified by the given amount to
   * improve accuracy.
   *
   * @param numSimulations The number of additional simulations.
   * @return The estimated price of the option.
   */
  double runMoreSimulations(unsigned long numSimulations);

  /**
   * @brief Gets the standard error of the Monte Carlo estimate.
   *
   * @return The standard error of the price estimate
   */
  double getStandardError() const;

  /**
   * @brief Gets the execution time of the last pricing calculation.
   *
   * @return The execution time in seconds
   */
  double getLastRunDuration() const;

  /**
   * @brief Calculates Value at Risk (VaR) at a 5% confidence level.
   *
   * @return The 5% VaR of the option payoff distribution
   */
  double calculateVaR() const;

  /**
   * @brief Checks if the option price has been calculated.
   *
   * @return True if calculatePrice() has been called successfully
   */
  bool isPriceCalculated() const;

 private:
  /**
   * @brief Simulates one stock price path to maturity.
   *
   * Uses geometric Brownian motion to evolve the stock price
   * from current price to maturity.
   *
   * @return Final stock price at option maturity
   */
  double simulatePath();

  /**
   * @brief Validates that price calculation has been performed.
   *
   * @throws std::runtime_error if price hasn't been calculated
   */
  void validatePriceCalculated() const;
};

#endif  // MONTECARLO_H
