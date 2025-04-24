// SPDX-License-Identifier: MIT

#include <cmath>
#include <gtest/gtest.h>
#include <thales/models/black_scholes_model.hpp>

using namespace thales::models;

// Helper function to compare floating point values with a tolerance
namespace {
inline bool is_close(double a, double b, double tolerance = 0.0001) {
    return std::abs(a - b) < tolerance;
}
} // namespace

class BlackScholesModelGreeksTest : public ::testing::Test {
  protected:
    // Common test parameters
    double S = 100.0;        // Current stock price
    double K = 100.0;        // Strike price
    double r = 0.05;         // Risk-free rate (5%)
    double sigma = 0.2;      // Volatility (20%)
    double T = 1.0;          // Time to expiration (1 year)
    double tolerance = 0.01; // Tolerance for floating point comparisons
};

TEST_F(BlackScholesModelGreeksTest, DeltaTest) {
    // Test call delta (ATM)
    double call_delta = BlackScholesModel::call_delta(S, K, r, sigma, T);
    EXPECT_TRUE(is_close(call_delta, 0.63, tolerance));

    // Test put delta (ATM)
    double put_delta = BlackScholesModel::put_delta(S, K, r, sigma, T);
    EXPECT_TRUE(is_close(put_delta, -0.37, tolerance));

    // Test call delta (ITM)
    call_delta = BlackScholesModel::call_delta(110.0, K, r, sigma, T);
    EXPECT_TRUE(is_close(call_delta, 0.73, tolerance));

    // Test put delta (ITM)
    put_delta = BlackScholesModel::put_delta(90.0, K, r, sigma, T);
    EXPECT_TRUE(is_close(put_delta, -0.73, tolerance));

    // Test call delta (OTM)
    call_delta = BlackScholesModel::call_delta(90.0, K, r, sigma, T);
    EXPECT_TRUE(is_close(call_delta, 0.27, tolerance));

    // Test put delta (OTM)
    put_delta = BlackScholesModel::put_delta(110.0, K, r, sigma, T);
    EXPECT_TRUE(is_close(put_delta, -0.27, tolerance));

    // Test put-call delta parity
    EXPECT_TRUE(is_close(BlackScholesModel::call_delta(S, K, r, sigma, T) -
                             BlackScholesModel::put_delta(S, K, r, sigma, T),
                         1.0, tolerance));
}

TEST_F(BlackScholesModelGreeksTest, GammaTest) {
    // Test gamma (same for call and put)
    double gamma = BlackScholesModel::gamma(S, K, r, sigma, T);
    EXPECT_TRUE(is_close(gamma, 0.019, tolerance));

    // Test gamma with different volatilities
    double gamma_low_vol = BlackScholesModel::gamma(S, K, r, 0.1, T);
    double gamma_high_vol = BlackScholesModel::gamma(S, K, r, 0.3, T);

    // Gamma should be higher for lower volatility when ATM
    EXPECT_GT(gamma_low_vol, gamma);
    EXPECT_LT(gamma_high_vol, gamma);

    // Test gamma with different time to expiration
    double gamma_short_time = BlackScholesModel::gamma(S, K, r, sigma, 0.25);
    double gamma_long_time = BlackScholesModel::gamma(S, K, r, sigma, 2.0);

    // Gamma should be higher for shorter time to expiration when ATM
    EXPECT_GT(gamma_short_time, gamma);
    EXPECT_LT(gamma_long_time, gamma);

    // Test gamma for ITM and OTM options
    double gamma_itm = BlackScholesModel::gamma(110.0, K, r, sigma, T);
    double gamma_otm = BlackScholesModel::gamma(90.0, K, r, sigma, T);

    // Gamma should be lower for ITM and OTM options compared to ATM
    EXPECT_LT(gamma_itm, gamma);
    EXPECT_LT(gamma_otm, gamma);

    // Gamma should be symmetric around ATM
    EXPECT_TRUE(is_close(gamma_itm, gamma_otm, tolerance));
}

TEST_F(BlackScholesModelGreeksTest, VegaTest) {
    // Test vega (same for call and put)
    double vega = BlackScholesModel::vega(S, K, r, sigma, T);
    EXPECT_TRUE(is_close(vega, 0.39, tolerance));

    // Test vega with different time to expiration
    double vega_short_time = BlackScholesModel::vega(S, K, r, sigma, 0.25);
    double vega_long_time = BlackScholesModel::vega(S, K, r, sigma, 2.0);

    // Vega should be higher for longer time to expiration
    EXPECT_LT(vega_short_time, vega);
    EXPECT_GT(vega_long_time, vega);

    // Test vega for ITM and OTM options
    double vega_itm = BlackScholesModel::vega(110.0, K, r, sigma, T);
    double vega_otm = BlackScholesModel::vega(90.0, K, r, sigma, T);

    // Vega should be lower for ITM and OTM options compared to ATM
    EXPECT_LT(vega_itm, vega);
    EXPECT_LT(vega_otm, vega);

    // Vega should be symmetric around ATM
    EXPECT_TRUE(is_close(vega_itm, vega_otm, tolerance));
}

TEST_F(BlackScholesModelGreeksTest, ThetaTest) {
    // Test call theta
    double call_theta = BlackScholesModel::call_theta(S, K, r, sigma, T);
    EXPECT_TRUE(is_close(call_theta, -0.05, tolerance));

    // Test put theta
    double put_theta = BlackScholesModel::put_theta(S, K, r, sigma, T);
    EXPECT_TRUE(is_close(put_theta, -0.03, tolerance));

    // Test call theta with different time to expiration
    double call_theta_short_time =
        BlackScholesModel::call_theta(S, K, r, sigma, 0.25);
    double call_theta_long_time =
        BlackScholesModel::call_theta(S, K, r, sigma, 2.0);

    // Theta should be more negative for shorter time to expiration
    EXPECT_LT(call_theta_short_time, call_theta);
    EXPECT_GT(call_theta_long_time, call_theta);

    // Test put theta with different time to expiration
    double put_theta_short_time =
        BlackScholesModel::put_theta(S, K, r, sigma, 0.25);
    double put_theta_long_time =
        BlackScholesModel::put_theta(S, K, r, sigma, 2.0);

    // Theta should be more negative for shorter time to expiration
    EXPECT_LT(put_theta_short_time, put_theta);
    EXPECT_GT(put_theta_long_time, put_theta);
}

TEST_F(BlackScholesModelGreeksTest, RhoTest) {
    // Test call rho
    double call_rho = BlackScholesModel::call_rho(S, K, r, sigma, T);
    EXPECT_TRUE(is_close(call_rho, 0.50, tolerance));

    // Test put rho
    double put_rho = BlackScholesModel::put_rho(S, K, r, sigma, T);
    EXPECT_TRUE(is_close(put_rho, -0.44, tolerance));

    // Test call rho with different time to expiration
    double call_rho_short_time =
        BlackScholesModel::call_rho(S, K, r, sigma, 0.25);
    double call_rho_long_time =
        BlackScholesModel::call_rho(S, K, r, sigma, 2.0);

    // Rho should be higher for longer time to expiration
    EXPECT_LT(call_rho_short_time, call_rho);
    EXPECT_GT(call_rho_long_time, call_rho);

    // Test put rho with different time to expiration
    double put_rho_short_time =
        BlackScholesModel::put_rho(S, K, r, sigma, 0.25);
    double put_rho_long_time = BlackScholesModel::put_rho(S, K, r, sigma, 2.0);

    // Rho should be more negative for longer time to expiration
    EXPECT_GT(put_rho_short_time, put_rho);
    EXPECT_LT(put_rho_long_time, put_rho);
}

TEST_F(BlackScholesModelGreeksTest, GreeksAtExpirationTest) {
    // At expiration (T = 0), for ATM options:
    double T_expiry = 0.0001; // Very close to expiration

    // Delta should approach 0.5 for ATM call and -0.5 for ATM put
    double call_delta = BlackScholesModel::call_delta(S, K, r, sigma, T_expiry);
    double put_delta = BlackScholesModel::put_delta(S, K, r, sigma, T_expiry);
    EXPECT_TRUE(is_close(call_delta, 0.5, 0.1));
    EXPECT_TRUE(is_close(put_delta, -0.5, 0.1));

    // Gamma should be very high for ATM options
    double gamma = BlackScholesModel::gamma(S, K, r, sigma, T_expiry);
    EXPECT_GT(gamma, 0.1);

    // Theta should be very negative for ATM options
    double call_theta = BlackScholesModel::call_theta(S, K, r, sigma, T_expiry);
    double put_theta = BlackScholesModel::put_theta(S, K, r, sigma, T_expiry);
    EXPECT_LT(call_theta, -0.1);
    EXPECT_LT(put_theta, -0.1);

    // Vega should approach 0
    double vega = BlackScholesModel::vega(S, K, r, sigma, T_expiry);
    EXPECT_TRUE(is_close(vega, 0.0, 0.1));

    // Rho should approach 0
    double call_rho = BlackScholesModel::call_rho(S, K, r, sigma, T_expiry);
    double put_rho = BlackScholesModel::put_rho(S, K, r, sigma, T_expiry);
    EXPECT_TRUE(is_close(call_rho, 0.0, 0.1));
    EXPECT_TRUE(is_close(put_rho, 0.0, 0.1));
}

TEST_F(BlackScholesModelGreeksTest, GreeksDeepITMOTMTest) {
    // Deep ITM call (S >> K)
    double S_itm = 150.0;
    double call_delta_itm =
        BlackScholesModel::call_delta(S_itm, K, r, sigma, T);
    double put_delta_itm = BlackScholesModel::put_delta(S_itm, K, r, sigma, T);

    // Delta should approach 1 for deep ITM call and 0 for deep ITM put
    EXPECT_TRUE(is_close(call_delta_itm, 1.0, 0.05));
    EXPECT_TRUE(is_close(put_delta_itm, 0.0, 0.05));

    // Deep OTM call (S << K)
    double S_otm = 50.0;
    double call_delta_otm =
        BlackScholesModel::call_delta(S_otm, K, r, sigma, T);
    double put_delta_otm = BlackScholesModel::put_delta(S_otm, K, r, sigma, T);

    // Delta should approach 0 for deep OTM call and -1 for deep OTM put
    EXPECT_TRUE(is_close(call_delta_otm, 0.0, 0.05));
    EXPECT_TRUE(is_close(put_delta_otm, -1.0, 0.05));

    // Gamma should be very low for both deep ITM and OTM options
    double gamma_itm = BlackScholesModel::gamma(S_itm, K, r, sigma, T);
    double gamma_otm = BlackScholesModel::gamma(S_otm, K, r, sigma, T);
    EXPECT_LT(gamma_itm, 0.005);
    EXPECT_LT(gamma_otm, 0.005);

    // Vega should be very low for both deep ITM and OTM options
    double vega_itm = BlackScholesModel::vega(S_itm, K, r, sigma, T);
    double vega_otm = BlackScholesModel::vega(S_otm, K, r, sigma, T);
    EXPECT_LT(vega_itm, 0.1);
    EXPECT_LT(vega_otm, 0.1);
}
