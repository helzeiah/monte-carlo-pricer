#ifndef OPTION_H
#define OPTION_H
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
    double stockPrice{}; // S
    double strikePrice{}; // K
    double maturityTime{}; // T
    double riskFreeRate{}; // r
    double volatility{}; // sigma
    OptionType type{}; // call/put

public:
    /**
     * Constructs an @code Option@endcode object with the given
     * Black-Scholes parameters.
     *
     * @param S the stock price
     * @param K the strike price
     * @param T the time until maturity
     * @param r the risk free rate
     * @param sigma the volatility
     * @param optType the type of the option (put / call).
     */
    Option(double S, double K, double T, double r, double sigma, OptionType optType);

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
     * @brief Outputs the details of this option to the standard output.
     *
     * Displays the option's Black-Scholes parameters
     * including stock price, strike price, time to maturity,
     * risk-free rate, volatility, and the option type (CALL or PUT).
     */
    void printDetails() const;
};
#endif
