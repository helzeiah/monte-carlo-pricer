#include "MonteCarlo.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <ranges>

MonteCarlo::MonteCarlo(const Option& option, unsigned long numSimulations,
                       unsigned int seed)
    : option{option},
      numSimulations{numSimulations},
      randomEngine{seed},
      standardNormal(0.0, 1.0),
      stockPrice{option.getStockPrice()},
      sqrtTimeToMaturity{std::sqrt(option.getTimeToMaturity())},
      driftPerSim{(option.getRiskFreeRate() -
                   0.5 * option.getVolatility() * option.getVolatility()) *
                  option.getTimeToMaturity()},
      volTimesSqrtT{option.getVolatility() * sqrtTimeToMaturity} {
  if (numSimulations == 0) {
    throw std::invalid_argument("Number of simulations must be positive.");
  }
  payoffs.reserve(numSimulations);
}

MonteCarlo::MonteCarlo(const Option& option, unsigned long numSimulations)
    : MonteCarlo(option, numSimulations, std::random_device()()) {}

const Option& MonteCarlo::getOption() const { return option; }

unsigned long MonteCarlo::getNumSimulations() const { return numSimulations; }

double MonteCarlo::calculatePrice() {
  if (priceCalculated) {
    return cachedPrice;
  }

  auto start = std::chrono::high_resolution_clock::now();

  const double discountFactor{
      std::exp(-option.getRiskFreeRate() * option.getTimeToMaturity())};

  double sumPayoffs{0.0};
  payoffs.resize(numSimulations);

  for (unsigned long i = 0; i < numSimulations; ++i) {
    double finalPrice{simulatePath()};
    payoffs[i] = option.calculatePayoff(finalPrice);
    sumPayoffs += payoffs[i];
  }

  cachedPrice = (sumPayoffs / numSimulations) * discountFactor;
  priceCalculated = true;

  auto end = std::chrono::high_resolution_clock::now();
  lastRunDuration = end - start;

  return cachedPrice;
}

std::pair<double, double> MonteCarlo::getConfidenceInterval() const {
  validatePriceCalculated();
  double marginOfError{1.96 * getStandardError()};
  return {cachedPrice - marginOfError, cachedPrice + marginOfError};
}

double MonteCarlo::runMoreSimulations(unsigned long additionalSimulations) {
  validatePriceCalculated();

  unsigned long oldNumSimulations = numSimulations;
  const double discountFactor =
      std::exp(-option.getRiskFreeRate() * option.getTimeToMaturity());
  double oldSumPayoffs = cachedPrice * oldNumSimulations / discountFactor;

  numSimulations += additionalSimulations;

  auto start = std::chrono::high_resolution_clock::now();

  double newSumPayoffs{0.0};
  payoffs.resize(numSimulations);

  for (unsigned long i = oldNumSimulations; i < numSimulations; ++i) {
    double finalPrice{simulatePath()};
    payoffs[i] = option.calculatePayoff(finalPrice);
    newSumPayoffs += payoffs[i];
  }

  cachedPrice =
      ((oldSumPayoffs + newSumPayoffs) / numSimulations) * discountFactor;

  auto end = std::chrono::high_resolution_clock::now();
  lastRunDuration = end - start;

  return cachedPrice;
}

double MonteCarlo::getStandardError() const {
  validatePriceCalculated();

  double mean = 0.0;
  for (double payoff : payoffs) {
    mean += payoff;
  }
  mean /= payoffs.size();

  double variance = 0.0;
  for (double payoff : payoffs) {
    variance += (payoff - mean) * (payoff - mean);
  }
  variance /= (payoffs.size() - 1);

  return std::sqrt(variance / payoffs.size());
}

double MonteCarlo::getLastRunDuration() const {
  return lastRunDuration.count();
}

double MonteCarlo::calculateVaR() const {
  validatePriceCalculated();

  std::vector<double> sortedPayoffs{payoffs};
  size_t varIndex{static_cast<size_t>(0.05 * sortedPayoffs.size())};
  std::ranges::nth_element(sortedPayoffs, sortedPayoffs.begin() + varIndex);
  return sortedPayoffs[varIndex];
}

bool MonteCarlo::isPriceCalculated() const { return priceCalculated; }

double MonteCarlo::simulatePath() {
  double rand{standardNormal(randomEngine)};
  return stockPrice * std::exp(driftPerSim + volTimesSqrtT * rand);
}

void MonteCarlo::validatePriceCalculated() const {
  if (!priceCalculated) {
    throw std::runtime_error(
        "Price must be calculated first. Call calculatePrice()");
  }
}
