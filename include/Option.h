#ifndef OPTION_H
#define OPTION_H
#include <string>
/**
 * @brief Represents the types of an Option.
 *
 * Represents the two standard types of financial options:
 * CALL (buy option) and PUT (sell option).
 */
enum class OptionType { CALL, PUT };

/**
 * @brief European option representation.
 *
 * This class represents a European-style option (exercisable only at maturity).
 * It includes the standard Black-Scholes parameters.
 */
class Option {
  OptionType type{};       // call/put
  double stockPrice{};     // S
  double strikePrice{};    // K
  double maturityTime{};   // T
  double riskFreeRate{};   // r
  double volatility{};     // sigma
  double dividendYield{};  // q

 public:
  /**
   * Constructs an @code Option@endcode object with the given
   * Black-Scholes parameters.
   *
   * @param optType the type of the option (put / call).
   * @param S the stock price
   * @param K the strike price
   * @param T the time until maturity
   * @param r the risk free rate
   * @param sigma the volatility
   * @param q the dividend yield
   */
  Option(OptionType optType, double S, double K, double T, double r,
         double sigma, double q = 0.0);

  /**
   * @brief Gets the type of this option.
   *
   * Returns the type of the option, indicating whether it is a
   * CALL (buy option) or PUT (sell option).
   *
   * @return The type of the option as an OptionType object.
   */
  OptionType getType() const;

  /**
   * @brief Gets the current stock price (S) of this option.
   *
   * Returns the price of the underlying asset associated with the option.
   *
   * @return The stock price of the option as a double.
   */
  double getStockPrice() const;

  /**
   * @brief Gets the strike price of the option.
   *
   * Returns the strike price (K) specified in this option's contract.
   *
   * @return The strike price of the option.
   */
  double getStrikePrice() const;

  /**
   * @brief Gets the time to maturity for this option.
   *
   * Returns the duration, in years, until this option's expiration.
   *
   * @return The time to maturity of the option.
   */
  double getTimeToMaturity() const;

  /**
   * @brief Gets the risk-free interest rate used in this option's model.
   *
   * Returns the risk-free rate used in the valuation of the option.
   *
   * @return The risk-free interest rate.
   */
  double getRiskFreeRate() const;

  /**
   * @brief Gets the volatility of this option.
   *
   * Returns the volatility (sigma) associated with this option.
   *
   * @return The volatility of the option.
   */
  double getVolatility() const;

  /**
   * @brief Gets the dividend yield of this option.
   *
   * Returns the dividend yield (q) associated with this option.
   *
   * @return The dividend yield.
   */
  double getDividendYield() const;

  /**
   * @brief Calculates the payoff of the option at maturity.
   *
   * Computes the payoff of the option based on its type (CALL or PUT)
   * and the given final price of the underlying asset.
   *
   * @param finalPrice The price of the underlying asset at maturity.
   * @return The payoff of the option.
   */
  double calculatePayoff(double finalPrice) const;

  /**
   *
   *@brief Overloaded stream insertion operator for Option.
   */
  friend std::ostream& operator<<(std::ostream& os, const Option& option);

  /**
   * @brief Static factory method for creating call options
   *
   * Provides type-safe construction. Eliminates the possibility of accidentally
   * inputting OptionType::PUT when creating a call option.
   *
   * @param S Stock price
   * @param K Strike price
   * @param T Time to maturity
   * @param r Risk-free rate
   * @param sigma volatility
   * @param q Dividend Yield
   * @return a Call option with specified parameters
   */
  static Option createCall(double S, double K, double T, double r,
                           double sigma, double q = 0.0);

  /**
   * @brief Static factory method for creating put options
   *
   * Provides type-safe construction. Eliminates the possibility of accidentally
   * inputting OptionType::CALL when creating a call option.
   *
   * @param S Stock price
   * @param K Strike price
   * @param T Time to maturity
   * @param r Risk-free rate
   * @param sigma volatility
   * @param q Dividend Yield
   * @return a Call option with specified parameters
   */
  static Option createPut(double S, double K, double T, double r,
                          double sigma, double q = 0.0);
};

#endif
