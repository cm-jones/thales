// SPDX-License-Identifier: MIT

#pragma once

#include <random>
#include <vector>

namespace thales {
namespace models {

/**
 * @struct OptionParameters
 * @brief Parameters for option pricing calculations
 */
struct OptionParameters {
    double S;             ///< Current price of the underlying asset
    double K;             ///< Strike price of the option
    double r;             ///< Risk-free interest rate
    double sigma;         ///< Volatility of the underlying asset
    double T;             ///< Time to expiration in years
    int num_simulations;  ///< Number of Monte Carlo simulations
    int num_steps;        ///< Number of time steps in each simulation
    unsigned int seed;    ///< Random number generator seed

    /**
     * @brief Constructor with default values for simulation parameters
     */
    OptionParameters(double S, double K, double r, double sigma, double T,
                     int num_simulations = 10000, int num_steps = 100,
                     unsigned int seed = 0)
        : S(S),
          K(K),
          r(r),
          sigma(sigma),
          T(T),
          num_simulations(num_simulations),
          num_steps(num_steps),
          seed(seed) {}
};

/**
 * @class MonteCarloModel
 * @brief Implementation of the Monte Carlo option pricing model.
 *
 * This class provides methods for calculating option prices using Monte Carlo
 * simulation. It supports both European call and put options.
 */
class MonteCarloModel {
   public:
    /**
     * @brief Calculate the price of a European call option using Monte Carlo
     * simulation
     * @param params Option parameters
     * @return The price of the call option
     */
    static double call_price(const OptionParameters &params);

    /**
     * @brief Calculate the price of a European put option using Monte Carlo
     * simulation
     * @param params Option parameters
     * @return The price of the put option
     */
    static double put_price(const OptionParameters &params);

    /**
     * @brief Calculate the delta of a European call option using Monte Carlo
     * simulation
     * @param params Option parameters
     * @return The delta of the call option
     */
    static double call_delta(const OptionParameters &params);

    /**
     * @brief Calculate the delta of a European put option using Monte Carlo
     * simulation
     * @param params Option parameters
     * @return The delta of the put option
     */
    static double put_delta(const OptionParameters &params);

    /**
     * @brief Calculate the gamma of an option using Monte Carlo simulation
     * @param params Option parameters
     * @return The gamma of the option
     */
    static double gamma(const OptionParameters &params);

    /**
     * @brief Calculate the vega of an option using Monte Carlo simulation
     * @param params Option parameters
     * @return The vega of the option
     */
    static double vega(const OptionParameters &params);

    // Legacy methods with individual parameters for backward compatibility
    static double call_price_legacy(double S, double K, double r, double sigma,
                                    double T, int num_simulations = 10000,
                                    int num_steps = 100, unsigned int seed = 0);

    static double put_price_legacy(double S, double K, double r, double sigma,
                                   double T, int num_simulations = 10000,
                                   int num_steps = 100, unsigned int seed = 0);

    static double call_delta_legacy(double S, double K, double r, double sigma,
                                    double T, int num_simulations = 10000,
                                    int num_steps = 100, unsigned int seed = 0);

    static double put_delta_legacy(double S, double K, double r, double sigma,
                                   double T, int num_simulations = 10000,
                                   int num_steps = 100, unsigned int seed = 0);

    static double gamma_legacy(double S, double K, double r, double sigma,
                               double T, int num_simulations = 10000,
                               int num_steps = 100, unsigned int seed = 0);

    static double vega_legacy(double S, double K, double r, double sigma,
                              double T, int num_simulations = 10000,
                              int num_steps = 100, unsigned int seed = 0);

   private:
    static double simulate_path(double S, double r, double sigma, double T,
                                int num_steps, std::mt19937 &gen);

    static double calculate_payoff(double S_T, double K, bool is_call);

    static double calculate_greek(double S, double K, double r, double sigma,
                                  double T, int num_simulations, int num_steps,
                                  unsigned int seed,
                                  double (*price_func)(double, double, double,
                                                       double, double, int, int,
                                                       unsigned int),
                                  double delta);
};

}  // namespace models
}  // namespace thales
