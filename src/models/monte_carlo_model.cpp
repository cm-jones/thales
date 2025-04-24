// SPDX-License-Identifier: MIT

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>
#include <thales/models/monte_carlo_model.hpp>

namespace thales {
namespace models {

// New methods using OptionParameters struct
double MonteCarloModel::call_price(const OptionParameters &params) {
    return call_price_legacy(params.S, params.K, params.r, params.sigma,
                             params.T, params.num_simulations, params.num_steps,
                             params.seed);
}

double MonteCarloModel::put_price(const OptionParameters &params) {
    return put_price_legacy(params.S, params.K, params.r, params.sigma,
                            params.T, params.num_simulations, params.num_steps,
                            params.seed);
}

double MonteCarloModel::call_delta(const OptionParameters &params) {
    return call_delta_legacy(params.S, params.K, params.r, params.sigma,
                             params.T, params.num_simulations, params.num_steps,
                             params.seed);
}

double MonteCarloModel::put_delta(const OptionParameters &params) {
    return put_delta_legacy(params.S, params.K, params.r, params.sigma,
                            params.T, params.num_simulations, params.num_steps,
                            params.seed);
}

double MonteCarloModel::gamma(const OptionParameters &params) {
    return gamma_legacy(params.S, params.K, params.r, params.sigma, params.T,
                        params.num_simulations, params.num_steps, params.seed);
}

double MonteCarloModel::vega(const OptionParameters &params) {
    return vega_legacy(params.S, params.K, params.r, params.sigma, params.T,
                       params.num_simulations, params.num_steps, params.seed);
}

// Legacy methods with individual parameters
double MonteCarloModel::call_price_legacy(double S, double K, double r,
                                          double sigma, double T,
                                          int num_simulations, int num_steps,
                                          unsigned int seed) {
    if (T <= 0) {
        // For expired options, return intrinsic value
        return std::max(0.0, S - K);
    }

    // Initialize random number generator
    std::mt19937 gen(seed);
    std::normal_distribution<double> dist(0.0, 1.0);

    double sum_payoffs = 0.0;
    for (int i = 0; i < num_simulations; ++i) {
        double S_T = simulate_path(S, r, sigma, T, num_steps, gen);
        sum_payoffs += calculate_payoff(S_T, K, true);
    }

    // Calculate the average payoff and discount it
    double avg_payoff = sum_payoffs / num_simulations;
    return std::exp(-r * T) * avg_payoff;
}

double MonteCarloModel::put_price_legacy(double S, double K, double r,
                                         double sigma, double T,
                                         int num_simulations, int num_steps,
                                         unsigned int seed) {
    if (T <= 0) {
        // For expired options, return intrinsic value
        return std::max(0.0, K - S);
    }

    // Initialize random number generator
    std::mt19937 gen(seed);
    std::normal_distribution<double> dist(0.0, 1.0);

    double sum_payoffs = 0.0;
    for (int i = 0; i < num_simulations; ++i) {
        double S_T = simulate_path(S, r, sigma, T, num_steps, gen);
        sum_payoffs += calculate_payoff(S_T, K, false);
    }

    // Calculate the average payoff and discount it
    double avg_payoff = sum_payoffs / num_simulations;
    return std::exp(-r * T) * avg_payoff;
}

double MonteCarloModel::call_delta_legacy(double S, double K, double r,
                                          double sigma, double T,
                                          int num_simulations, int num_steps,
                                          unsigned int seed) {
    // Delta is calculated as the partial derivative of the option price with
    // respect to S We use a small perturbation of S to approximate the
    // derivative
    const double delta = 0.01 * S; // 1% of S
    return calculate_greek(S, K, r, sigma, T, num_simulations, num_steps, seed,
                           call_price_legacy, delta);
}

double MonteCarloModel::put_delta_legacy(double S, double K, double r,
                                         double sigma, double T,
                                         int num_simulations, int num_steps,
                                         unsigned int seed) {
    // Delta is calculated as the partial derivative of the option price with
    // respect to S We use a small perturbation of S to approximate the
    // derivative
    const double delta = 0.01 * S; // 1% of S
    return calculate_greek(S, K, r, sigma, T, num_simulations, num_steps, seed,
                           put_price_legacy, delta);
}

double MonteCarloModel::gamma_legacy(double S, double K, double r, double sigma,
                                     double T, int num_simulations,
                                     int num_steps, unsigned int seed) {
    // Gamma is calculated as the second partial derivative of the option price
    // with respect to S We use a small perturbation of S to approximate the
    // derivative
    const double delta = 0.01 * S; // 1% of S

    // Calculate price at S+delta and S-delta
    double price_up = call_price_legacy(S + delta, K, r, sigma, T,
                                        num_simulations, num_steps, seed);
    double price_down = call_price_legacy(S - delta, K, r, sigma, T,
                                          num_simulations, num_steps, seed);
    double price_center =
        call_price_legacy(S, K, r, sigma, T, num_simulations, num_steps, seed);

    // Second derivative approximation
    return (price_up + price_down - 2 * price_center) / (delta * delta);
}

double MonteCarloModel::vega_legacy(double S, double K, double r, double sigma,
                                    double T, int num_simulations,
                                    int num_steps, unsigned int seed) {
    // Vega is calculated as the partial derivative of the option price with
    // respect to sigma We use a small perturbation of sigma to approximate the
    // derivative
    const double delta = 0.01; // 1% change in volatility

    // Calculate price at sigma+delta and sigma-delta
    double price_up = call_price_legacy(S, K, r, sigma + delta, T,
                                        num_simulations, num_steps, seed);
    double price_down = call_price_legacy(S, K, r, sigma - delta, T,
                                          num_simulations, num_steps, seed);

    // First derivative approximation
    return (price_up - price_down) / (2 * delta) /
           100.0; // Divided by 100 to get per 1% change
}

double MonteCarloModel::simulate_path(double S, double r, double sigma,
                                      double T, int num_steps,
                                      std::mt19937 &gen) {
    std::normal_distribution<double> dist(0.0, 1.0);

    double dt = T / num_steps;
    double S_t = S;

    for (int i = 0; i < num_steps; ++i) {
        double z = dist(gen);
        S_t *= std::exp((r - 0.5 * sigma * sigma) * dt +
                        sigma * std::sqrt(dt) * z);
    }

    return S_t;
}

double MonteCarloModel::calculate_payoff(double S_T, double K, bool is_call) {
    if (is_call) {
        return std::max(0.0, S_T - K);
    } else {
        return std::max(0.0, K - S_T);
    }
}

double MonteCarloModel::calculate_greek(
    double S, double K, double r, double sigma, double T, int num_simulations,
    int num_steps, unsigned int seed,
    double (*price_func)(double, double, double, double, double, int, int,
                         unsigned int),
    double delta) {

    // Calculate price at S+delta and S-delta
    double price_up =
        price_func(S + delta, K, r, sigma, T, num_simulations, num_steps, seed);
    double price_down =
        price_func(S - delta, K, r, sigma, T, num_simulations, num_steps, seed);

    // First derivative approximation
    return (price_up - price_down) / (2 * delta);
}

} // namespace models
} // namespace thales
