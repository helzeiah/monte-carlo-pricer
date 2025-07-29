#ifndef IMPLIEDVOL_H
#define IMPLIEDVOL_H

#include "Option.h"

/**
 * @brief Solves for implied volatility under the Black-Scholes model.
 *
 * Uses a Newton-Raphson root-finding (with bisection fallback) to find
 * the volatility σ such that BS_price(option, σ) = targetPrice.
 *
 * @param option      the Option parameters (S, K, T, r, q, type).
 * @param targetPrice the market price to invert.
 * @param tol         the convergence tolerance (default: 1e-8).
 * @param maxIter     the maximum number of Newton iterations (default: 50).
 * @return the implied volatility σ.
 * @throws std::runtime_error if convergence fails within maxIter.
 */
double impliedVolBS(const Option& option, double targetPrice, double tol = 1e-8,
                    int maxIter = 50);

#endif  // IMPLIEDVOL_H
