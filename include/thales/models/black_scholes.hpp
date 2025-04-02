#ifndef THALES_MODELS_BLACK_SCHOLES_HPP
#define THALES_MODELS_BLACK_SCHOLES_HPP

#include <string>

namespace thales {
namespace models {

/**
 * @class BlackScholes
 * @brief Implementation of the Black-Scholes option pricing model.
 *
 * This class provides methods for calculating option prices and Greeks
 * using the Black-Scholes formula.
 */
class BlackScholes {
   public:
    /**
     * @brief Calculate the price of a European call option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The price of the call option
     */
    static double call_price(double S, double K, double r, double sigma,
                             double T);

    /**
     * @brief Calculate the price of a European put option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The price of the put option
     */
    static double put_price(double S, double K, double r, double sigma,
                            double T);

    /**
     * @brief Calculate the delta of a European call option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The delta of the call option
     */
    static double call_delta(double S, double K, double r, double sigma,
                             double T);

    /**
     * @brief Calculate the delta of a European put option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The delta of the put option
     */
    static double put_delta(double S, double K, double r, double sigma,
                            double T);

    /**
     * @brief Calculate the gamma of an option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The gamma of the option
     */
    static double gamma(double S, double K, double r, double sigma, double T);

    /**
     * @brief Calculate the vega of an option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The vega of the option
     */
    static double vega(double S, double K, double r, double sigma, double T);

    /**
     * @brief Calculate the theta of a European call option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The theta of the call option
     */
    static double call_theta(double S, double K, double r, double sigma,
                             double T);

    /**
     * @brief Calculate the theta of a European put option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The theta of the put option
     */
    static double put_theta(double S, double K, double r, double sigma,
                            double T);

    /**
     * @brief Calculate the rho of a European call option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The rho of the call option
     */
    static double call_rho(double S, double K, double r, double sigma,
                           double T);

    /**
     * @brief Calculate the rho of a European put option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The rho of the put option
     */
    static double put_rho(double S, double K, double r, double sigma, double T);

    /**
     * @brief Calculate the implied volatility of a European call option
     * @param price The market price of the call option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param T The time to expiration in years
     * @param epsilon The precision of the calculation
     * @param maxIterations The maximum number of iterations
     * @return The implied volatility
     */
    static double call_implied_volatility(double price, double S, double K,
                                          double r, double T,
                                          double epsilon = 0.0001,
                                          int max_iterations = 100);

    /**
     * @brief Calculate the implied volatility of a European put option
     * @param price The market price of the put option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param T The time to expiration in years
     * @param epsilon The precision of the calculation
     * @param maxIterations The maximum number of iterations
     * @return The implied volatility
     */
    static double put_implied_volatility(double price, double S, double K,
                                         double r, double T,
                                         double epsilon = 0.0001,
                                         int max_iterations = 100);

   private:
    // Helper functions
    static double d1(double S, double K, double r, double sigma, double T);
    static double d2(double S, double K, double r, double sigma, double T);
    static double normal_cdf(double x);
    static double normal_pdf(double x);
};

}  // namespace models
}  // namespace thales

#endif  // THALES_MODELS_BLACK_SCHOLES_HPP
