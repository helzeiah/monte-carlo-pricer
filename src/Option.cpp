#include "Option.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

Option::Option(OptionType optType, double S, double K, double T, double r,
               double sigma, double q)
    : type(optType),
      stockPrice(S),
      strikePrice(K),
      maturityTime(T),
      riskFreeRate(r),
      volatility(sigma),
      dividendYield(q) {
  // validate parameters, all must be positive except risk-free rate
  // (can be negative in some cases)
  if (S <= 0 || K <= 0 || T <= 0 || sigma <= 0) {
    throw std::invalid_argument(
        "Parameters S, K, T, and sigma must be positive.");
  }
  if (q < 0.0) {
    throw std::invalid_argument("Dividend yield q must be >= 0.");
  }
}

OptionType Option::getType() const { return type; }

double Option::getStockPrice() const { return stockPrice; }

double Option::getStrikePrice() const { return strikePrice; }

double Option::getTimeToMaturity() const { return maturityTime; }

double Option::getRiskFreeRate() const { return riskFreeRate; }

double Option::getVolatility() const { return volatility; }

double Option::getDividendYield() const { return dividendYield; }

double Option::calculatePayoff(double finalPrice) const {
  if (type == OptionType::CALL) {
    // Call payoff -> max(S_T - K, 0)
    return std::max(finalPrice - strikePrice, 0.0);
  }  // Put payoff -> max(K - S_T, 0)
  return std::max(strikePrice - finalPrice, 0.0);
}

std::ostream& operator<<(std::ostream& os, const Option& option) {
  os << std::fixed << std::setprecision(2);
  os << "Option:\n"
     << "Type: " << (option.type == OptionType::CALL ? "Call" : "Put") << "\n"
     << "Stock Price: " << option.stockPrice << "\n"
     << "Strike Price: " << option.strikePrice << "\n"
     << "Time to Maturity: " << option.maturityTime << " years\n"
     << "Risk Free Rate: " << option.riskFreeRate * 100 << "%\n"
     << "Volatility: " << option.volatility * 100 << "%\n"
     << "Dividend Yield: " << option.dividendYield * 100 << "%\n";
  return os;
}

Option Option::createCall(double S, double K, double T, double r, double sigma,
                          double q) {
  return {OptionType::CALL, S, K, T, r, sigma, q};
}

Option Option::createPut(double S, double K, double T, double r, double sigma,
                         double q) {
  return {OptionType::PUT, S, K, T, r, sigma, q};
}
