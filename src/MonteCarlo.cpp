#include "MonteCarlo.h"

#include <algorithm>
#include <cmath>
#include <ranges>
#include <stdexcept>

MonteCarlo::MonteCarlo(const Option& option, unsigned long numSimulations,
                       unsigned int seed)
    : Pricer{option},
      numSimulations{numSimulations},
      randomEngine{seed},
      standardNormal(0.0, 1.0),
      stockPrice{option.getStockPrice()},
      sqrtTimeToMaturity{std::sqrt(option.getTimeToMaturity())},
      // pre-calculate drift term for efficiency: (r - σ²/2) * T
      driftPerSim{(option.getRiskFreeRate() -
                   0.5 * option.getVolatility() * option.getVolatility()) *
                  option.getTimeToMaturity()},
      // pre-calculate volatility scaling: σ * √T
      volTimesSqrtT{option.getVolatility() * sqrtTimeToMaturity} {
  if (numSimulations == 0) {
    throw std::invalid_argument("Number of simulations must be positive.");
  }
  // reserve memory upfront to avoid repeated allocations
  payoffs.reserve(numSimulations);
}

// Constructor delegates to seeded, using random device for total randomness
MonteCarlo::MonteCarlo(const Option& option, unsigned long numSimulations)
    : MonteCarlo(option, numSimulations, std::random_device()()) {}

unsigned long MonteCarlo::getNumSimulations() const { return numSimulations; }

double MonteCarlo::calculatePrice() const {
  // if price is calculated -> dont calculate again
  if (priceCalculated) {
    return cachedPrice;
  }

  // track start of calculation to measure performance
  auto start = std::chrono::high_resolution_clock::now();

  // calculate discount factor once: e^(-r*T)
  const double discountFactor{
      std::exp(-option.getRiskFreeRate() * option.getTimeToMaturity())};

  double sumPayoffs{0.0};
  payoffs.resize(numSimulations);

  // Main loop - each simulation calculates and stores payoff, adding to sum
  for (unsigned long i = 0; i < numSimulations; ++i) {
    double finalPrice{simulatePath()};
    payoffs[i] = option.calculatePayoff(finalPrice);
    sumPayoffs += payoffs[i];
  }

  // the estimate -> average payoff * discount factor
  cachedPrice = (sumPayoffs / numSimulations) * discountFactor;
  priceCalculated = true;

  // end clock and store the duration of the simulation
  auto end = std::chrono::high_resolution_clock::now();
  lastRunDuration = end - start;

  return cachedPrice;
}

std::string MonteCarlo::getPricingMethod() const {
  return "Monte Carlo Pricer";
}

Greeks MonteCarlo::calculateGreeks() {
  //TODO: stub for now
  return {0.5, 0.02, -0.01, 0.15, 0.03};
}

std::pair<double, double> MonteCarlo::getConfidenceInterval(double confidenceLevel) {
  validatePriceCalculated();

  if (confidenceLevel <= 0.0 || confidenceLevel >= 1.0) {
    throw std::invalid_argument("Confidence level must be between 0 and 1");
  }

  double zScore;
  if (confidenceLevel >= 0.99) {
    zScore = 2.576;
  } else if (confidenceLevel >= 0.95) {
    zScore = 1.96;
  } else if (confidenceLevel >= 0.90) {
    zScore = 1.645;
  } else {
    zScore = 1.282;
  }
  // 1.96 = z-score for 95% confidence interval on a normal distribution
  double marginOfError{zScore * getStandardError()};
  // we want left an right bounds
  return {cachedPrice - marginOfError, cachedPrice + marginOfError};
}

double MonteCarlo::getStandardError() {
  validatePriceCalculated();

  // calculate sample mean
  double mean = 0.0;
  for (double payoff : payoffs) {
    mean += payoff;
  }
  mean /= payoffs.size();

  // calculate sample variance
  double variance = 0.0;
  for (double payoff : payoffs) {
    variance += (payoff - mean) * (payoff - mean);
  }
  // divide by n-1 to negate bias (Bessel's correction)
  variance /= (payoffs.size() - 1);

  // standard error = standard deviation / sqrt(n)
  return std::sqrt(variance / payoffs.size());
}

double MonteCarlo::calculateVaR(double confidenceLevel) {
  validatePriceCalculated();

  // create copy to avoid modifying original
  std::vector<double> sortedPayoffs{payoffs};
  // the position of the 5% quantile
  size_t varIndex{static_cast<size_t>(confidenceLevel * sortedPayoffs.size())};
  // partial sort to find single quantile - more efficient than full sort
  std::ranges::nth_element(sortedPayoffs, sortedPayoffs.begin() + varIndex);
  return sortedPayoffs[varIndex];
}

double MonteCarlo::runMoreSimulations(unsigned long additionalSimulations) {
  validatePriceCalculated();

  // store current state
  unsigned long oldNumSimulations = numSimulations;
  const double discountFactor =
      std::exp(-option.getRiskFreeRate() * option.getTimeToMaturity());

  // reverse-engineering of the undiscounted sum from the cached price
  double oldSumPayoffs = cachedPrice * oldNumSimulations / discountFactor;

  numSimulations += additionalSimulations;

  auto start = std::chrono::high_resolution_clock::now();

  double newSumPayoffs{0.0};
  // extend vector to new size
  payoffs.resize(numSimulations);

  // only simulate the additional paths
  for (unsigned long i = oldNumSimulations; i < numSimulations; ++i) {
    double finalPrice{simulatePath()};
    payoffs[i] = option.calculatePayoff(finalPrice);
    newSumPayoffs += payoffs[i];
  }

  // weighted average of old and new results
  cachedPrice =
      ((oldSumPayoffs + newSumPayoffs) / numSimulations) * discountFactor;

  auto end = std::chrono::high_resolution_clock::now();
  lastRunDuration = end - start;

  return cachedPrice;
}

double MonteCarlo::simulatePath() const {
  // generate random
  double rand{standardNormal(randomEngine)};
  // calculate price via Geometric Brownian Motion:
  // S(T) = S(0) * exp((drift + vol * Z))
  return stockPrice * std::exp(driftPerSim + volTimesSqrtT * rand);
}

void MonteCarlo::validatePriceCalculated() const {
  if (!priceCalculated) {
    throw std::runtime_error(
        "Price must be calculated first. Call calculatePrice()");
  }
}
