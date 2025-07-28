#include "ImpliedVol.h"

#include <cmath>
#include <stdexcept>

#include "BlackScholes.h"
#include "MathUtils.h"

double impliedVolBS(const Option& opt, double targetPrice, double tol,
                    int maxIter) {
  if (targetPrice <= 0.0)
    throw std::invalid_argument("Target price must be > 0");

  // initial guess: use Brenner-Subrahmanyam ATM approx
  double S = opt.getStockPrice(), K = opt.getStrikePrice();
  double T = opt.getTimeToMaturity(), r = opt.getRiskFreeRate();
  double q = opt.getDividendYield();
  double guess = std::sqrt(2.0 * std::fabs(std::log(S / K)) / T);
  if (guess <= 0.0 || std::isnan(guess)) guess = 0.2;

  double sigmaLow = 1e-6, sigmaHigh = 5.0;
  double sigma = guess;

  for (int i = 0; i < maxIter; ++i) {
    Option tmp(opt.getType(), S, K, T, r, sigma, q);
    BlackScholes bs(tmp);
    double price = bs.calculatePrice();
    double diff = price - targetPrice;

    if (std::fabs(diff) < tol) return sigma;

    // Vega
    double sqrtT = std::sqrt(T);
    double d1 =
        (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * sqrtT);
    double vega = S * std::exp(-q * T) * math::norm_pdf(d1) * sqrtT;
    if (vega < 1e-12) {  // fallback to bisection
      (diff > 0 ? sigmaHigh : sigmaLow) = sigma;
      sigma = 0.5 * (sigmaLow + sigmaHigh);
      continue;
    }

    double sigmaNew = sigma - diff / vega;
    if (sigmaNew < sigmaLow || sigmaNew > sigmaHigh || std::isnan(sigmaNew)) {
      // bisection step
      (diff > 0 ? sigmaHigh : sigmaLow) = sigma;
      sigma = 0.5 * (sigmaLow + sigmaHigh);
    } else {
      sigma = sigmaNew;
    }
  }
  return sigma;  // best effort
}