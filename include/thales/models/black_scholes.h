#ifndef THALES_MODELS_BLACK_SCHOLES_H
#define THALES_MODELS_BLACK_SCHOLES_H

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
    static double callPrice(double S, double K, double r, double sigma, double T);
    
    /**
     * @brief Calculate the price of a European put option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The price of the put option
     */
    static double putPrice(double S, double K, double r, double sigma, double T);
    
    /**
     * @brief Calculate the delta of a European call option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The delta of the call option
     */
    static double callDelta(double S, double K, double r, double sigma, double T);
    
    /**
     * @brief Calculate the delta of a European put option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The delta of the put option
     */
    static double putDelta(double S, double K, double r, double sigma, double T);
    
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
    static double callTheta(double S, double K, double r, double sigma, double T);
    
    /**
     * @brief Calculate the theta of a European put option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The theta of the put option
     */
    static double putTheta(double S, double K, double r, double sigma, double T);
    
    /**
     * @brief Calculate the rho of a European call option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The rho of the call option
     */
    static double callRho(double S, double K, double r, double sigma, double T);
    
    /**
     * @brief Calculate the rho of a European put option
     * @param S The current price of the underlying asset
     * @param K The strike price of the option
     * @param r The risk-free interest rate
     * @param sigma The volatility of the underlying asset
     * @param T The time to expiration in years
     * @return The rho of the put option
     */
    static double putRho(double S, double K, double r, double sigma, double T);
    
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
    static double callImpliedVolatility(
        double price, double S, double K, double r, double T,
        double epsilon = 0.0001, int maxIterations = 100
    );
    
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
    static double putImpliedVolatility(
        double price, double S, double K, double r, double T,
        double epsilon = 0.0001, int maxIterations = 100
    );

private:
    // Helper functions
    static double d1(double S, double K, double r, double sigma, double T);
    static double d2(double S, double K, double r, double sigma, double T);
    static double normalCDF(double x);
    static double normalPDF(double x);
};

} // namespace models
} // namespace thales

#endif // THALES_MODELS_BLACK_SCHOLES_H
