#include <benchmark/benchmark.h>

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include "thales/models/black_scholes_model.hpp"
#include "thales/models/black_scholes_model_vectorized.hpp"

using namespace thales::models;

// Helper function to generate random test data
void generate_test_data(int size, std::vector<double> &S,
                        std::vector<double> &K, std::vector<double> &r,
                        std::vector<double> &sigma, std::vector<double> &T) {
    std::random_device rd;
    std::mt19937 gen(rd());

    // Distributions for realistic option parameters
    std::uniform_real_distribution<> S_dist(50.0, 200.0);  // Stock price
    std::uniform_real_distribution<> K_dist(50.0, 200.0);  // Strike price
    std::uniform_real_distribution<> r_dist(0.01,
                                            0.05);  // Risk-free rate (1-5%)
    std::uniform_real_distribution<> sigma_dist(0.1,
                                                0.5);  // Volatility (10-50%)
    std::uniform_real_distribution<> T_dist(
        0.1,
        2.0);  // Time to expiry (0.1-2 years)

    // Resize vectors
    S.resize(size);
    K.resize(size);
    r.resize(size);
    sigma.resize(size);
    T.resize(size);

    // Generate random data
    for (int i = 0; i < size; ++i) {
        S[i] = S_dist(gen);
        K[i] = K_dist(gen);
        r[i] = r_dist(gen);
        sigma[i] = sigma_dist(gen);
        T[i] = T_dist(gen);
    }
}

// Benchmark for scalar call_price function
static void BM_BlackScholesCallPrice_Scalar(benchmark::State &state) {
    // Number of options to price in each iteration
    const int num_options = state.range(0);

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Output vector for results
    std::vector<double> results(num_options);

    // Main benchmark loop
    for (auto _ : state) {
        for (int i = 0; i < num_options; ++i) {
            results[i] =
                BlackScholesModel::call_price(S[i], K[i], r[i], sigma[i], T[i]);
        }
        benchmark::DoNotOptimize(results);
        benchmark::ClobberMemory();
    }

    // Set items processed per second
    state.SetItemsProcessed(int64_t(state.iterations()) * num_options);
}

// Benchmark for vectorized call_price function
static void BM_BlackScholesCallPrice_Vectorized(benchmark::State &state) {
    // Check if AVX is supported
    if (!BlackScholesVectorized::cpu_supports_avx()) {
        state.SkipWithError("AVX not supported on this CPU");
        return;
    }

    // Number of options to price in each iteration
    const int num_options = state.range(0);

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Output vector for results
    std::vector<double> results(num_options);

    // Main benchmark loop
    for (auto _ : state) {
        BlackScholesVectorized::vectorized_call_price(
            S.data(), K.data(), r.data(), sigma.data(), T.data(),
            results.data(), num_options);
        benchmark::DoNotOptimize(results);
        benchmark::ClobberMemory();
    }

    // Set items processed per second
    state.SetItemsProcessed(int64_t(state.iterations()) * num_options);
}

// Benchmark for scalar put_price function
static void BM_BlackScholesPutPrice_Scalar(benchmark::State &state) {
    // Number of options to price in each iteration
    const int num_options = state.range(0);

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Output vector for results
    std::vector<double> results(num_options);

    // Main benchmark loop
    for (auto _ : state) {
        for (int i = 0; i < num_options; ++i) {
            results[i] =
                BlackScholesModel::put_price(S[i], K[i], r[i], sigma[i], T[i]);
        }
        benchmark::DoNotOptimize(results);
        benchmark::ClobberMemory();
    }

    // Set items processed per second
    state.SetItemsProcessed(int64_t(state.iterations()) * num_options);
}

// Benchmark for vectorized put_price function
static void BM_BlackScholesPutPrice_Vectorized(benchmark::State &state) {
    // Check if AVX is supported
    if (!BlackScholesVectorized::cpu_supports_avx()) {
        state.SkipWithError("AVX not supported on this CPU");
        return;
    }

    // Number of options to price in each iteration
    const int num_options = state.range(0);

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Output vector for results
    std::vector<double> results(num_options);

    // Main benchmark loop
    for (auto _ : state) {
        BlackScholesVectorized::vectorized_put_price(
            S.data(), K.data(), r.data(), sigma.data(), T.data(),
            results.data(), num_options);
        benchmark::DoNotOptimize(results);
        benchmark::ClobberMemory();
    }

    // Set items processed per second
    state.SetItemsProcessed(int64_t(state.iterations()) * num_options);
}

// Benchmark for scalar call_delta function
static void BM_BlackScholesCallDelta_Scalar(benchmark::State &state) {
    // Number of options to process in each iteration
    const int num_options = state.range(0);

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Output vector for results
    std::vector<double> results(num_options);

    // Main benchmark loop
    for (auto _ : state) {
        for (int i = 0; i < num_options; ++i) {
            results[i] =
                BlackScholesModel::call_delta(S[i], K[i], r[i], sigma[i], T[i]);
        }
        benchmark::DoNotOptimize(results);
        benchmark::ClobberMemory();
    }

    // Set items processed per second
    state.SetItemsProcessed(int64_t(state.iterations()) * num_options);
}

// Benchmark for vectorized call_delta function
static void BM_BlackScholesCallDelta_Vectorized(benchmark::State &state) {
    // Check if AVX is supported
    if (!BlackScholesVectorized::cpu_supports_avx()) {
        state.SkipWithError("AVX not supported on this CPU");
        return;
    }

    // Number of options to process in each iteration
    const int num_options = state.range(0);

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Output vector for results
    std::vector<double> results(num_options);

    // Main benchmark loop
    for (auto _ : state) {
        BlackScholesVectorized::vectorized_call_delta(
            S.data(), K.data(), r.data(), sigma.data(), T.data(),
            results.data(), num_options);
        benchmark::DoNotOptimize(results);
        benchmark::ClobberMemory();
    }

    // Set items processed per second
    state.SetItemsProcessed(int64_t(state.iterations()) * num_options);
}

// Benchmark for scalar gamma function
static void BM_BlackScholesGamma_Scalar(benchmark::State &state) {
    // Number of options to process in each iteration
    const int num_options = state.range(0);

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Output vector for results
    std::vector<double> results(num_options);

    // Main benchmark loop
    for (auto _ : state) {
        for (int i = 0; i < num_options; ++i) {
            results[i] =
                BlackScholesModel::gamma(S[i], K[i], r[i], sigma[i], T[i]);
        }
        benchmark::DoNotOptimize(results);
        benchmark::ClobberMemory();
    }

    // Set items processed per second
    state.SetItemsProcessed(int64_t(state.iterations()) * num_options);
}

// Benchmark for vectorized gamma function
static void BM_BlackScholesGamma_Vectorized(benchmark::State &state) {
    // Check if AVX is supported
    if (!BlackScholesVectorized::cpu_supports_avx()) {
        state.SkipWithError("AVX not supported on this CPU");
        return;
    }

    // Number of options to process in each iteration
    const int num_options = state.range(0);

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Output vector for results
    std::vector<double> results(num_options);

    // Main benchmark loop
    for (auto _ : state) {
        BlackScholesVectorized::vectorized_gamma(S.data(), K.data(), r.data(),
                                                 sigma.data(), T.data(),
                                                 results.data(), num_options);
        benchmark::DoNotOptimize(results);
        benchmark::ClobberMemory();
    }

    // Set items processed per second
    state.SetItemsProcessed(int64_t(state.iterations()) * num_options);
}

// Batch size test case - call price
// This shows the effect of different batch sizes on vectorization performance
static void BM_BlackScholesCallPrice_BatchTest(benchmark::State &state) {
    // Batch size from benchmark parameter
    const int batch_size = state.range(0);

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(batch_size, S, K, r, sigma, T);

    // Output vectors for both scalar and vectorized implementations
    std::vector<double> scalar_results(batch_size);
    std::vector<double> vector_results(batch_size);

    for (auto _ : state) {
        // Scalar implementation
        for (int i = 0; i < batch_size; ++i) {
            scalar_results[i] =
                BlackScholesModel::call_price(S[i], K[i], r[i], sigma[i], T[i]);
        }

        // Vectorized implementation (if supported)
        if (BlackScholesVectorized::cpu_supports_avx()) {
            BlackScholesVectorized::vectorized_call_price(
                S.data(), K.data(), r.data(), sigma.data(), T.data(),
                vector_results.data(), batch_size);
        }

        benchmark::DoNotOptimize(scalar_results);
        benchmark::DoNotOptimize(vector_results);
        benchmark::ClobberMemory();
    }

    // Verify results match (within epsilon)
    if (BlackScholesVectorized::cpu_supports_avx()) {
        const double epsilon = 1e-10;
        for (int i = 0; i < batch_size; ++i) {
            if (std::abs(scalar_results[i] - vector_results[i]) > epsilon) {
                state.SkipWithError(
                    "Validation failed: scalar and vector results differ");
                break;
            }
        }
    }

    state.SetItemsProcessed(int64_t(state.iterations()) * batch_size);
}

// Register the benchmarks with Google Benchmark
BENCHMARK(BM_BlackScholesCallPrice_Scalar)
    ->Arg(4)      // Minimum size (one AVX vector)
    ->Arg(16)     // Small batch
    ->Arg(64)     // Medium batch
    ->Arg(1024)   // Large batch
    ->Arg(4096);  // Very large batch

BENCHMARK(BM_BlackScholesCallPrice_Vectorized)
    ->Arg(4)      // Minimum size (one AVX vector)
    ->Arg(16)     // Small batch
    ->Arg(64)     // Medium batch
    ->Arg(1024)   // Large batch
    ->Arg(4096);  // Very large batch

BENCHMARK(BM_BlackScholesPutPrice_Scalar)
    ->Arg(4)      // Minimum size (one AVX vector)
    ->Arg(64)     // Medium batch
    ->Arg(1024);  // Large batch

BENCHMARK(BM_BlackScholesPutPrice_Vectorized)
    ->Arg(4)      // Minimum size (one AVX vector)
    ->Arg(64)     // Medium batch
    ->Arg(1024);  // Large batch

BENCHMARK(BM_BlackScholesCallDelta_Scalar)
    ->Arg(4)      // Minimum size (one AVX vector)
    ->Arg(64)     // Medium batch
    ->Arg(1024);  // Large batch

BENCHMARK(BM_BlackScholesCallDelta_Vectorized)
    ->Arg(4)      // Minimum size (one AVX vector)
    ->Arg(64)     // Medium batch
    ->Arg(1024);  // Large batch

BENCHMARK(BM_BlackScholesGamma_Scalar)
    ->Arg(4)      // Minimum size (one AVX vector)
    ->Arg(64)     // Medium batch
    ->Arg(1024);  // Large batch

BENCHMARK(BM_BlackScholesGamma_Vectorized)
    ->Arg(4)      // Minimum size (one AVX vector)
    ->Arg(64)     // Medium batch
    ->Arg(1024);  // Large batch

BENCHMARK(BM_BlackScholesCallPrice_BatchTest)
    ->Arg(4)      // Minimum size (one AVX vector)
    ->Arg(16)     // Small batch
    ->Arg(64)     // Medium batch
    ->Arg(256)    // Larger batch
    ->Arg(1024);  // Very large batch

BENCHMARK_MAIN();
