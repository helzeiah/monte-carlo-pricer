#include <iomanip>
#include <iostream>

#include "BlackScholes.h"
#include "MonteCarlo.h"
#include "Option.h"

int main() {
  try {
    // Static factory methods for call/puts
    auto callOption = Option::createCall(100.0, 105.0, 1.0, 0.05, 0.20);
    std::cout << "\n=== Inheritance & Polymorphism ===\n";
    // MonteCarlo inherits from Pricer
    MonteCarlo mcPricer(callOption);
    BlackScholes bsPricer(callOption);

    std::cout << "\n=== Monte Carlo Results ===\n";
    // Calculate option price
    double price = mcPricer.calculatePrice();
    std::cout << "Option Price MC: $" << std::fixed << std::setprecision(2)
              << price << '\n';
    std::cout << "MC Run took: " << mcPricer.getLastCalculationTime()
    << " seconds" << '\n';
    price = bsPricer.calculatePrice();  // Black-Scholes is also a Pricer
    std::cout << "Option Price BS: $" << std::fixed << std::setprecision(2)
              << price << '\n';
    std::cout << "BS Run took: " << bsPricer.getLastCalculationTime()
    << " seconds" << '\n';


    // Get confidence interval
    auto [lower, upper] = mcPricer.getConfidenceInterval();
    std::cout << "95% Confidence Interval: [$" << lower << ", $" << upper << "]"
              << '\n';

    // Get standard error
    std::cout << "Standard Error: $" << mcPricer.getStandardError() << '\n';

    // Calculate VaR
    std::cout << "5% VaR: $" << mcPricer.calculateVaR() << '\n';

    // Greeks calculation (pure virtual function)
    Greeks greeks = mcPricer.calculateGreeks();
    std::cout << greeks << std::endl;  // operator<< for Greeks

    // Run more simulations for better accuracy
    std::cout << "\nRunning 50,000 more simulations..." << '\n';
    double newPrice = mcPricer.runMoreSimulations(50000);
    std::cout << "Updated Price: $" << newPrice << '\n';

    std::cout << "Additional run took: " << mcPricer.getLastCalculationTime()
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