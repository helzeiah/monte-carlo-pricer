#include <iostream>
#include <memory>
#include <iomanip>

#include "Option.h"
#include "Pricer.h"
#include "BlackScholes.h"
#include "MonteCarlo.h"

int main() {
  // define a European Call option: S=100, K=100, T=1yr, r=5%, σ=20%
  Option opt = Option::createCall(100.0, 100.0, 1.0, 0.05, 0.20);

  // create polymorphic pricers
  std::unique_ptr<Pricer> bsPricer = std::make_unique<BlackScholes>(opt);
  std::unique_ptr<Pricer> mcPricer = std::make_unique<MonteCarlo>(opt, 100000, 42u);

  // Black-Scholes pricing
  double bsPrice = bsPricer->calculatePrice();
  std::cout << bsPricer->getPricingMethod()
            << " price: $" << std::fixed << std::setprecision(2)
            << bsPrice << '\n';

  // Monte Carlo pricing + 95% confidence interval
  double mcPrice = mcPricer->calculatePrice();
  auto [low, hi] = mcPricer->getConfidenceInterval();
  std::cout << mcPricer->getPricingMethod()
            << " price: $" << std::fixed << std::setprecision(2)
            << mcPrice
            << " 95% CI: [" << low << ", " << hi << "]"
            << '\n';

  return 0;
}