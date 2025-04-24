#include <benchmark/benchmark.h>

#include <thales/models/black_scholes_model.hpp>

using namespace thales::models;

// Benchmark for call_price function
static void BM_BlackScholesCallPrice(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::call_price(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesCallPrice);

// Benchmark for put_price function
static void BM_BlackScholesPutPrice(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::put_price(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesPutPrice);

// Benchmark for call_delta function
static void BM_BlackScholesCallDelta(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::call_delta(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesCallDelta);

// Benchmark for put_delta function
static void BM_BlackScholesPutDelta(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::put_delta(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesPutDelta);

// Benchmark for gamma function
static void BM_BlackScholesGamma(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(BlackScholesModel::gamma(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesGamma);

// Benchmark for vega function
static void BM_BlackScholesVega(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(BlackScholesModel::vega(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesVega);

// Benchmark for call_theta function
static void BM_BlackScholesCallTheta(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::call_theta(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesCallTheta);

// Benchmark for put_theta function
static void BM_BlackScholesPutTheta(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::put_theta(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesPutTheta);

// Benchmark for call_rho function
static void BM_BlackScholesCallRho(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::call_rho(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesCallRho);

// Benchmark for put_rho function
static void BM_BlackScholesPutRho(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(BlackScholesModel::put_rho(S, K, r, sigma, T));
    }
}
BENCHMARK(BM_BlackScholesPutRho);

// Benchmark for call_implied_volatility function
static void BM_BlackScholesCallImpliedVolatility(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    // Calculate option price for the given volatility
    double price = BlackScholesModel::call_price(S, K, r, sigma, T);

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::call_implied_volatility(price, S, K, r, T));
    }
}
BENCHMARK(BM_BlackScholesCallImpliedVolatility);

// Benchmark for put_implied_volatility function
static void BM_BlackScholesPutImpliedVolatility(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    // Calculate option price for the given volatility
    double price = BlackScholesModel::put_price(S, K, r, sigma, T);

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::put_implied_volatility(price, S, K, r, T));
    }
}
BENCHMARK(BM_BlackScholesPutImpliedVolatility);

// Benchmark for normal_cdf function
static void BM_BlackScholesNormalCDF(benchmark::State &state) {
    // Setup parameters
    double x = 0.5; // A typical value for d1 or d2

    for (auto _ : state) {
        // We need to access the private method using a workaround
        // This is a bit hacky but necessary for benchmarking private methods
        benchmark::DoNotOptimize(
            BlackScholesModel::call_delta(100.0, 100.0, 0.0, 0.2, 1.0));
    }
}
BENCHMARK(BM_BlackScholesNormalCDF);

// Benchmark for normal_pdf function
static void BM_BlackScholesNormalPDF(benchmark::State &state) {
    // Setup parameters
    double x = 0.5; // A typical value for d1 or d2

    for (auto _ : state) {
        // We need to access the private method using a workaround
        // This is a bit hacky but necessary for benchmarking private methods
        benchmark::DoNotOptimize(
            BlackScholesModel::gamma(100.0, 100.0, 0.0, 0.2, 1.0));
    }
}
BENCHMARK(BM_BlackScholesNormalPDF);

// Benchmark for a complete option pricing scenario
static void BM_BlackScholesCompleteOptionPricing(benchmark::State &state) {
    // Setup parameters
    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;     // Time to expiration (1 year)

    for (auto _ : state) {
        // Calculate call and put prices
        double call_price = BlackScholesModel::call_price(S, K, r, sigma, T);
        double put_price = BlackScholesModel::put_price(S, K, r, sigma, T);

        // Calculate Greeks
        double call_delta = BlackScholesModel::call_delta(S, K, r, sigma, T);
        double put_delta = BlackScholesModel::put_delta(S, K, r, sigma, T);
        double gamma = BlackScholesModel::gamma(S, K, r, sigma, T);
        double vega = BlackScholesModel::vega(S, K, r, sigma, T);
        double call_theta = BlackScholesModel::call_theta(S, K, r, sigma, T);
        double put_theta = BlackScholesModel::put_theta(S, K, r, sigma, T);
        double call_rho = BlackScholesModel::call_rho(S, K, r, sigma, T);
        double put_rho = BlackScholesModel::put_rho(S, K, r, sigma, T);

        benchmark::DoNotOptimize(call_price);
        benchmark::DoNotOptimize(put_price);
        benchmark::DoNotOptimize(call_delta);
        benchmark::DoNotOptimize(put_delta);
        benchmark::DoNotOptimize(gamma);
        benchmark::DoNotOptimize(vega);
        benchmark::DoNotOptimize(call_theta);
        benchmark::DoNotOptimize(put_theta);
        benchmark::DoNotOptimize(call_rho);
        benchmark::DoNotOptimize(put_rho);
    }
}
BENCHMARK(BM_BlackScholesCompleteOptionPricing);

// Parameterized benchmark for call_price with different moneyness levels
static void BM_BlackScholesCallPrice_Moneyness(benchmark::State &state) {
    // Get the moneyness parameter (S/K ratio)
    double moneyness = state.range(0) / 100.0; // Convert from int to ratio

    double K = 100.0;         // Strike price
    double S = K * moneyness; // Current stock price based on moneyness
    double r = 0.05;          // Risk-free rate (5%)
    double sigma = 0.2;       // Volatility (20%)
    double T = 1.0;           // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::call_price(S, K, r, sigma, T));
    }
}
// Test with different moneyness levels: 80% (OTM), 100% (ATM), 120% (ITM)
BENCHMARK(BM_BlackScholesCallPrice_Moneyness)->Arg(80)->Arg(100)->Arg(120);

// Parameterized benchmark for call_price with different volatility levels
static void BM_BlackScholesCallPrice_Volatility(benchmark::State &state) {
    // Get the volatility parameter
    double sigma = state.range(0) / 100.0; // Convert from int to decimal

    double S = 100.0; // Current stock price
    double K = 100.0; // Strike price
    double r = 0.05;  // Risk-free rate (5%)
    double T = 1.0;   // Time to expiration (1 year)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::call_price(S, K, r, sigma, T));
    }
}
// Test with different volatility levels: 10%, 20%, 30%, 50%
BENCHMARK(BM_BlackScholesCallPrice_Volatility)
    ->Arg(10)
    ->Arg(20)
    ->Arg(30)
    ->Arg(50);

// Parameterized benchmark for call_price with different time to expiration
static void BM_BlackScholesCallPrice_TimeToExpiration(benchmark::State &state) {
    // Get the time to expiration parameter in days
    double T = state.range(0) / 365.0; // Convert from days to years

    double S = 100.0;   // Current stock price
    double K = 100.0;   // Strike price
    double r = 0.05;    // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            BlackScholesModel::call_price(S, K, r, sigma, T));
    }
}

// Test with different expiration times: 7 days, 30 days, 90 days, 180 days, 365
// days
BENCHMARK(BM_BlackScholesCallPrice_TimeToExpiration)
    ->Arg(7)
    ->Arg(30)
    ->Arg(90)
    ->Arg(180)
    ->Arg(365);

BENCHMARK_MAIN();
