#include "BlackScholes.h"
#include "MathUtils.h"

double BlackScholes::calculatePrice() const {
  if (priceCalculated) {
    return cachedPrice;
  }

  auto start = std::chrono::high_resolution_clock::now();

  const double S = option.getStockPrice();
  const double K = option.getStrikePrice();
  const double T = option.getTimeToMaturity();
  const double r = option.getRiskFreeRate();
  const double sigma = option.getVolatility();
  const double q = option.getDividendYield();

  if (T <= 1e-12) {
    cachedPrice = option.calculatePayoff(S);
    priceCalculated = true;
    lastRunDuration = std::chrono::high_resolution_clock::now() - start;
    return cachedPrice;
  }

  if (sigma <= 1e-12) {
    const double forward = S * std::exp((r - q) * T);
    const double payoff = option.calculatePayoff(forward);
    cachedPrice = payoff * std::exp(-r * T);
    priceCalculated = true;
    lastRunDuration = std::chrono::high_resolution_clock::now() - start;
    return cachedPrice;
  }
  const double sqrtT = std::sqrt(T);
  const double logSK = std::log(S / K);
  const double discR = std::exp(-r * T);
  const double discQ = std::exp(-q * T);

  const double d1 =
      (logSK + (r - q + 0.5 * sigma * sigma) * T) / (sigma * sqrtT);
  const double d2 = d1 - sigma * sqrtT;

  if (option.getType() == OptionType::CALL) {
    cachedPrice =
        S * discQ * math::norm_cdf(d1) - K * discR * math::norm_cdf(d2);
  } else {
    cachedPrice =
        K * discR * math::norm_cdf(-d2) - S * discQ * math::norm_cdf(-d1);
  }
  priceCalculated = true;
  lastRunDuration = std::chrono::high_resolution_clock::now() - start;
  return cachedPrice;
}

Greeks BlackScholes::calculateGreeks() {
  const double S = option.getStockPrice();
  const double K = option.getStrikePrice();
  const double T = option.getTimeToMaturity();
  const double r = option.getRiskFreeRate();
  const double sigma = option.getVolatility();
  const double q = option.getDividendYield();

  if (T <= 1e-12 || sigma <= 1e-12) {
    return Greeks{};
  }

  const double sqrtT = std::sqrt(T);
  const double d1 =
      (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * sqrtT);
  const double d2 = d1 - sigma * sqrtT;
  const double pdfd1 = math::norm_pdf(d1);
  const double discQ = std::exp(-q * T);
  const double discR = std::exp(-r * T);

  double delta{};
  double theta{};
  double rho{};
  if (option.getType() == OptionType::CALL) {
    delta = discQ * math::norm_cdf(d1);
    theta = (-S * discQ * pdfd1 * sigma / (2.0 * sqrtT)) -
            r * K * discR * math::norm_cdf(d2) +
            q * S * discQ * math::norm_cdf(d1);
    rho = K * T * discR * math::norm_cdf(d2);
  } else {
    delta = -discQ * math::norm_cdf(-d1);
    theta = (-S * discQ * pdfd1 * sigma / (2.0 * sqrtT)) +
            r * K * discR * math::norm_cdf(-d2) -
            q * S * discQ * math::norm_cdf(-d1);
    rho = -K * T * discR * math::norm_cdf(-d2);
  }
  const double gamma = (discQ * pdfd1) / (S * sigma * sqrtT);
  const double vega = S * discQ * pdfd1 * sqrtT;

  return Greeks{delta, gamma, theta, vega, rho};
}
