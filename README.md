![CI](https://github.com/helzeiah/monte-carlo-pricer/actions/workflows/ci.yml/badge.svg)
# European Options Pricing Library (C++)

A small C++20 library that prices European options via Black-Scholes (closed form) and Monte Carlo (Geometric Brownian Motion). This Library supports Greeks calculation, implied volatility estimation, and provides statistical outputs like standard error, confidence intervals, and VaR. It has clean abstractions with the Pricer interface and is unit-tested with GoogleTest.

---

## Features
* Pricing Engines
  * Black-Scholes analytic pricer (supports dividends)
  * Monte Carlo pricer (GBM) with cached normals, CI, SE, VaR, incremental runs.
* Greeks
  * BS: analytic Greeks
  * MC: finite-difference Greeks with common random numbers
* Implied volatility via hybrid Newton/Bisection
* Robust input validation
* Caching of computed prices + timing info
* Extensive GoogleTest suit (deterministic and stochastic assertions)
* Clean, documented headers suitable for reuse

---

## Project Structure
```monte-carlo-pricer/
├── CMakeLists.txt
├── include/
│   ├── BlackScholes.h
│   ├── Greeks.h
│   ├── ImpliedVol.h
│   ├── MathUtils.h
│   ├── MonteCarlo.h
│   ├── Option.h
│   └── Pricer.h
├── src/
│   ├── BlackScholes.cpp
│   ├── ImpliedVol.cpp
│   ├── MonteCarlo.cpp
│   ├── Option.cpp
│   └── main.cpp               # demo
├── tests/
│   ├── BlackScholesTest.cpp
│   ├── CachingAndStateTest.cpp
│   ├── ImpliedVolTest.cpp
│   ├── MathUtilsTest.cpp
│   ├── MonteCarloTest.cpp
│   ├── OptionTest.cpp
│   ├── ParamGridTest.cpp
│   ├── PricerInterfaceTest.cpp
│   ├── PutCallParityTest.cpp
│   └── TestUtils.h
├── docs/
│   └── Doxyfile               # Doxygen configuration
├── .github/
│   └── workflows/
│       └── ci.yml             # GitHub Actions (build + tests + docs)
├── LICENSE
└── README.md
```

---

## Getting Started
```bash
# clone
git clone https://github.com/helzeiah/monte-carlo-pricer.git
cd monte-carlo-pricer

# configure (Debug by default)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# build library, demo, and tests
cmake --build build -j
```

### Run the demo program

```bash
./build/proj_monte_carlo_pricer
```

### Run the tests

```bash
ctest --test-dir build --output-on-failure
# or directly
./build/unit_tests
```

---

## Quick Usage Example

```cpp
#include "Option.h"
#include "BlackScholes.h"
#include "MonteCarlo.h"
#include <iostream>

int main() {
    auto opt = Option::createCall(100, 100, 1.0, 0.05, 0.20, 0.0);

    BlackScholes bs(opt);
    std::cout << "BS Price: " << bs.calculatePrice() << "\n";
    std::cout << "BS Greeks: " << bs.calculateGreeks() << "\n";

    MonteCarlo mc(opt, 200000, 42u);
    double mcPrice = mc.calculatePrice();
    auto [lo, hi] = mc.getConfidenceInterval(0.95);
    std::cout << "MC Price: " << mcPrice << " (95% CI: [" << lo << ", " << hi << "])\n";

    return 0;
}
```

---

## API Snapshot

### `Option`

Represents a European option with parameters *(S, K, T, r, σ, q)*. Factory methods `createCall`, `createPut` ensure correctness. Can `calculatePayoff` given a final price.

### `Pricer` (abstract)

Base interface for pricing engines. Exposes `calculatePrice()`, `calculateGreeks()`, plus optional stats methods.

### `BlackScholes`

Closed-form pricer + analytic Greeks. Falls back gracefully for `T≈0` or `σ≈0`.

### `MonteCarlo`

GBM path simulator using one-step terminal price. Stores normals & payoffs for reuse. Supports:

- `getStandardError()`
- `getConfidenceInterval(alpha)`
- `calculateVaR(alpha)`
- `runMoreSimulations(n)`

### `impliedVolBS()`

Root-finds σ to match a target price. Starts with Brenner–Subrahmanyam ATM guess, then Newton + bisection fallback.

### `math::norm_pdf/norm_cdf`

Small, constexpr-friendly helpers for standard normal.

---

## Testing & Validation

- \~**X** unit tests across:
  - BS prices & Greeks vs reference values
  - MC vs BS with statistical tolerances (3×SE logic & deterministic fallback)
  - Put-call parity checks (analytic & MC)
  - Input validation / exception throwing
  - Caching/time measurements
- Deterministic seeds ensure reproducibility; heavy tests use higher path counts.

To add a test, drop a `*.cpp` file into `tests/`—CMake auto-discovers it.

---

## Performance Notes

- Pre-computed drift & σ√T per constructor
- Stores generated normals for Greek bumps (common random numbers)
- `runMoreSimulations()` adds paths without redoing old work

**Ideas to speed up**:

- Antithetic variates or control variates (use BS as control)
- Parallel loops (OpenMP, TBB)
- SIMD random generation (xoshiro / PCG + vectorized exp)

---

## Roadmap / Future Work

- Add Binomial trees for American-style options
- Implement Control Variates / Antithetic variates in MC
- Path-dependent payoffs (Asian Barrier)

---

## License
This project is licensed under the [MIT License](./LICENSE) © 2025 Helzeiah Oliveira

---

## Acknowledgements

* GoogleTest - unit-testing framework
* Black & Scholes (1973) and Hull, Options, Futures, and Other Derivatives - formulas used for reference values.

---

## Contact

Helzeiah Oliveira - helzeiaho@gmail.com

*Price that option for me 5 :)*






