// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <vector>

#include "thales/models/black_scholes_model.hpp"
#include "thales/models/black_scholes_model_vectorized.hpp"

using namespace thales::models;

// Helper to generate test data (same as in benchmark)
void generate_test_data(int size, std::vector<double> &S,
                        std::vector<double> &K, std::vector<double> &r,
                        std::vector<double> &sigma, std::vector<double> &T) {
    // Use fixed seed for reproducible tests
    std::mt19937 gen(42);

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

class BlackScholesVectorizedTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Skip the tests if AVX is not supported
        if (!BlackScholesVectorized::cpu_supports_avx()) {
            GTEST_SKIP() << "AVX not supported on this CPU";
        }
    }
};

// Test vectorized call price against scalar implementation
TEST_F(BlackScholesVectorizedTest, TestCallPrice) {
    const int num_options = 16;  // Test multiple options at once

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Compute scalar results
    std::vector<double> scalar_results(num_options);
    for (int i = 0; i < num_options; ++i) {
        scalar_results[i] =
            BlackScholesModel::call_price(S[i], K[i], r[i], sigma[i], T[i]);
    }

    // Compute vectorized results
    std::vector<double> vector_results(num_options);
    BlackScholesVectorized::vectorized_call_price(
        S.data(), K.data(), r.data(), sigma.data(), T.data(),
        vector_results.data(), num_options);

    // Compare results with tolerance
    const double epsilon = 1e-10;
    for (int i = 0; i < num_options; ++i) {
        EXPECT_NEAR(scalar_results[i], vector_results[i], epsilon)
            << "Difference at index " << i << ": "
            << "Scalar: " << scalar_results[i] << ", "
            << "Vector: " << vector_results[i];
    }
}

// Test vectorized put price against scalar implementation
TEST_F(BlackScholesVectorizedTest, TestPutPrice) {
    const int num_options = 16;

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Compute scalar results
    std::vector<double> scalar_results(num_options);
    for (int i = 0; i < num_options; ++i) {
        scalar_results[i] =
            BlackScholesModel::put_price(S[i], K[i], r[i], sigma[i], T[i]);
    }

    // Compute vectorized results
    std::vector<double> vector_results(num_options);
    BlackScholesVectorized::vectorized_put_price(
        S.data(), K.data(), r.data(), sigma.data(), T.data(),
        vector_results.data(), num_options);

    // Compare results with tolerance
    const double epsilon = 1e-10;
    for (int i = 0; i < num_options; ++i) {
        EXPECT_NEAR(scalar_results[i], vector_results[i], epsilon)
            << "Difference at index " << i << ": "
            << "Scalar: " << scalar_results[i] << ", "
            << "Vector: " << vector_results[i];
    }
}

// Test vectorized call delta against scalar implementation
TEST_F(BlackScholesVectorizedTest, TestCallDelta) {
    const int num_options = 16;

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Compute scalar results
    std::vector<double> scalar_results(num_options);
    for (int i = 0; i < num_options; ++i) {
        scalar_results[i] =
            BlackScholesModel::call_delta(S[i], K[i], r[i], sigma[i], T[i]);
    }

    // Compute vectorized results
    std::vector<double> vector_results(num_options);
    BlackScholesVectorized::vectorized_call_delta(
        S.data(), K.data(), r.data(), sigma.data(), T.data(),
        vector_results.data(), num_options);

    // Compare results with tolerance
    const double epsilon = 1e-10;
    for (int i = 0; i < num_options; ++i) {
        EXPECT_NEAR(scalar_results[i], vector_results[i], epsilon)
            << "Difference at index " << i << ": "
            << "Scalar: " << scalar_results[i] << ", "
            << "Vector: " << vector_results[i];
    }
}

// Test vectorized gamma against scalar implementation
TEST_F(BlackScholesVectorizedTest, TestGamma) {
    const int num_options = 16;

    // Generate random test data
    std::vector<double> S, K, r, sigma, T;
    generate_test_data(num_options, S, K, r, sigma, T);

    // Compute scalar results
    std::vector<double> scalar_results(num_options);
    for (int i = 0; i < num_options; ++i) {
        scalar_results[i] =
            BlackScholesModel::gamma(S[i], K[i], r[i], sigma[i], T[i]);
    }

    // Compute vectorized results
    std::vector<double> vector_results(num_options);
    BlackScholesVectorized::vectorized_gamma(
        S.data(), K.data(), r.data(), sigma.data(), T.data(),
        vector_results.data(), num_options);

    // Compare results with tolerance
    const double epsilon = 1e-10;
    for (int i = 0; i < num_options; ++i) {
        EXPECT_NEAR(scalar_results[i], vector_results[i], epsilon)
            << "Difference at index " << i << ": "
            << "Scalar: " << scalar_results[i] << ", "
            << "Vector: " << vector_results[i];
    }
}

// Test edge cases (expired options)
TEST_F(BlackScholesVectorizedTest, TestExpiredOptions) {
    const int num_options = 4;

    // Create test data with expired options (T = 0)
    std::vector<double> S = {100.0, 90.0, 110.0, 100.0};
    std::vector<double> K = {100.0, 100.0, 100.0, 100.0};
    std::vector<double> r = {0.05, 0.05, 0.05, 0.05};
    std::vector<double> sigma = {0.2, 0.2, 0.2, 0.2};
    std::vector<double> T = {0.0, 0.0, 0.0, 0.0};

    // Compute scalar results
    std::vector<double> scalar_call_results(num_options);
    std::vector<double> scalar_put_results(num_options);
    for (int i = 0; i < num_options; ++i) {
        scalar_call_results[i] =
            BlackScholesModel::call_price(S[i], K[i], r[i], sigma[i], T[i]);
        scalar_put_results[i] =
            BlackScholesModel::put_price(S[i], K[i], r[i], sigma[i], T[i]);
    }

    // Compute vectorized results
    std::vector<double> vector_call_results(num_options);
    std::vector<double> vector_put_results(num_options);
    BlackScholesVectorized::vectorized_call_price(
        S.data(), K.data(), r.data(), sigma.data(), T.data(),
        vector_call_results.data(), num_options);
    BlackScholesVectorized::vectorized_put_price(
        S.data(), K.data(), r.data(), sigma.data(), T.data(),
        vector_put_results.data(), num_options);

    // Compare results with tolerance
    const double epsilon = 1e-10;
    for (int i = 0; i < num_options; ++i) {
        EXPECT_NEAR(scalar_call_results[i], vector_call_results[i], epsilon);
        EXPECT_NEAR(scalar_put_results[i], vector_put_results[i], epsilon);
    }

    // Verify intrinsic values for expired options
    EXPECT_NEAR(vector_call_results[0], 0.0, epsilon);   // ATM call (S = K)
    EXPECT_NEAR(vector_call_results[1], 0.0, epsilon);   // OTM call (S < K)
    EXPECT_NEAR(vector_call_results[2], 10.0, epsilon);  // ITM call (S > K)
    EXPECT_NEAR(vector_put_results[0], 0.0, epsilon);    // ATM put (S = K)
    EXPECT_NEAR(vector_put_results[1], 10.0, epsilon);   // ITM put (S < K)
    EXPECT_NEAR(vector_put_results[2], 0.0, epsilon);    // OTM put (S > K)
}

// Test with invalid inputs (negative T or sigma)
TEST_F(BlackScholesVectorizedTest, TestInvalidInputs) {
    const int num_options = 4;

    // Create test data with invalid inputs
    std::vector<double> S = {100.0, 100.0, 100.0, 100.0};
    std::vector<double> K = {100.0, 100.0, 100.0, 100.0};
    std::vector<double> r = {0.05, 0.05, 0.05, 0.05};
    std::vector<double> sigma = {0.2, -0.1, 0.0, 0.2};  // Invalid sigma
    std::vector<double> T = {1.0, 1.0, 1.0, -0.1};      // Invalid T

    // Compute scalar results (expecting NaN for invalid inputs)
    std::vector<double> scalar_results(num_options);
    for (int i = 0; i < num_options; ++i) {
        scalar_results[i] =
            BlackScholesModel::call_price(S[i], K[i], r[i], sigma[i], T[i]);
    }

    // Compute vectorized results
    std::vector<double> vector_results(num_options);
    BlackScholesVectorized::vectorized_call_price(
        S.data(), K.data(), r.data(), sigma.data(), T.data(),
        vector_results.data(), num_options);

    // Valid inputs should match
    EXPECT_NEAR(scalar_results[0], vector_results[0], 1e-10);

    // For invalid inputs, we expect similar behavior (both NaN or both valid)
    for (int i = 1; i < num_options; ++i) {
        bool scalar_is_nan = std::isnan(scalar_results[i]);
        bool vector_is_nan = std::isnan(vector_results[i]);
        EXPECT_EQ(scalar_is_nan, vector_is_nan)
            << "At index " << i << ": Scalar "
            << (scalar_is_nan ? "is" : "is not") << " NaN but Vector "
            << (vector_is_nan ? "is" : "is not") << " NaN";
    }
}
