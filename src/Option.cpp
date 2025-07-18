#include "Option.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

Option::Option(OptionType optType, double S, double K, double T, double r,
               double sigma)
    : type(optType),
      stockPrice(S),
      strikePrice(K),
      maturityTime(T),
      riskFreeRate(r),
      volatility(sigma) {
  // validate parameters, all must be positive except risk-free rate
  // (can be negative in some cases)
  if (S <= 0 || K <= 0 || T <= 0 || sigma <= 0) {
    throw std::invalid_argument(
        "Parameters S, K, T, and sigma must be positive.");
  }
}

OptionType Option::getType() const { return type; }

double Option::getStockPrice() const { return stockPrice; }

double Option::getStrikePrice() const { return strikePrice; }

double Option::getTimeToMaturity() const { return maturityTime; }

double Option::getRiskFreeRate() const { return riskFreeRate; }

double Option::getVolatility() const { return volatility; }

double Option::calculatePayoff(double finalPrice) const {
  if (type == OptionType::CALL) {
    // Call payoff -> max(S_T - K, 0)
    return std::max(finalPrice - strikePrice, 0.0);
  } // Put payoff -> max(K - S_T, 0)
    return std::max(strikePrice - finalPrice, 0.0);
}

std::string Option::toString() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2);
  ss << "Option:\n"
     << "Type: " << (type == OptionType::CALL ? "Call" : "Put") << "\n"
     << "Stock Price: " << stockPrice << "\n"
     << "Strike Price: " << strikePrice << "\n"
     << "Time to Maturity: " << maturityTime << " years\n"
     << "Risk Free Rate: " << riskFreeRate * 100 << "%\n"
     << "Volatility: " << volatility * 100 << "%\n";
  return ss.str();
}
