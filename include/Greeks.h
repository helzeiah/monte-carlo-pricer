#ifndef GREEKS_H
#define GREEKS_H
#include <iomanip>

/**
 * Represents the Greeks parameters often used in financial derivatives pricing
 * models. These parameters provide sensitivity measures of the derivative.
 */
struct Greeks {
  double delta{0.0};  // ∂V/∂S
  double gamma{0.0};  // ∂²V/∂S²
  double theta{0.0};  // ∂V/∂t
  double vega{0.0};   // ∂V/∂σ
  double rho{0.0};    // ∂V/∂r

  Greeks() = default;
  Greeks(double d, double g, double t, double v, double r)
      : delta(d), gamma(g), theta(t), vega(v), rho(r) {}

  /**
   * Overloads the stream insertion operator for the Greeks struct.
   * Outputs the values of the Greek parameters in a formatted string.
   *
   * @param os The output stream where the information is to be written.
   * @param greeks The Greeks object containing the delta, gamma, theta, vega,
   * and rho values to be displayed.
   * @return A reference to the modified output stream with the formatted Greeks
   * information inserted.
   */
  friend std::ostream& operator<<(std::ostream& os, const Greeks& greeks) {
    os << "Greeks: [delta=" << std::fixed << std::setprecision(4) << greeks.delta
       << " gamma=" << greeks.gamma
       << " theta=" << greeks.theta
       << " vega=" << greeks.vega
       << " rho=" << greeks.rho << "]";
    return os;
  }
};

#endif  // GREEKS_H
