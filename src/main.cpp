#include <iomanip>
#include <iostream>

#include "MonteCarlo.h"
#include "Option.h"

int main() {
  try {
    // Create a European Call option
    // S=100, K=105, T=1 year, r=5%, sigma=20%
    Option callOption(OptionType::CALL, 100.0, 105.0, 1.0, 0.05, 0.20);

    std::cout << callOption.toString() << std::endl;

    // Create Monte Carlo pricer with 100,000 simulations by default
    MonteCarlo mcPricer(callOption);

    // Calculate option price
    double price = mcPricer.calculatePrice();
    std::cout << "Option Price: $" << std::fixed << std::setprecision(2)
              << price << '\n';

    // Get confidence interval
    auto [lower, upper] = mcPricer.getConfidenceInterval();
    std::cout << "95% Confidence Interval: [$" << lower << ", $" << upper << "]"
              << '\n';

    // Get standard error
    std::cout << "Standard Error: $" << mcPricer.getStandardError()
              << '\n';

    // Calculate VaR
    std::cout << "5% VaR: $" << mcPricer.calculateVaR() << '\n';

    // Run more simulations for better accuracy
    std::cout << "\nRunning 50,000 more simulations..." << '\n';
    double newPrice = mcPricer.runMoreSimulations(50000);
    std::cout << "Updated Price: $" << newPrice << '\n';

    std::cout << "Last run took: " << mcPricer.getLastRunDuration()
              << " seconds" << '\n';

    // Demonstrate error handling
    std::cout << "\nDemonstrating error handling..." << std::endl;
    try {
      Option badOption(OptionType::PUT, -100, 50, 1, 0.05, 0.20);
    } catch (const std::invalid_argument& e) {
      std::cout << "Caught error: " << e.what() << std::endl;
    }

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}