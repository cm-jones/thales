// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <thales/models/monte_carlo_model.hpp>

namespace thales {
namespace tests {

class MonteCarloModelTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Set up common test parameters
        params.S = 100.0;               // Current stock price
        params.K = 100.0;               // Strike price
        params.r = 0.05;                // Risk-free rate
        params.sigma = 0.2;             // Volatility
        params.T = 1.0;                 // Time to expiration (1 year)
        params.num_simulations = 10000; // Number of simulations
        params.num_steps = 100;         // Number of time steps
        params.seed = 42;               // Random seed for reproducibility
    }

    models::OptionParameters params;
};

// Test call option pricing
TEST_F(MonteCarloModelTest, CallPrice) {
    // Test ATM call option
    double call_price = models::MonteCarloModel::call_price(params);
    EXPECT_GT(call_price, 0.0);
    EXPECT_LT(call_price,
              params.S); // Call price should be less than stock price

    // Test ITM call option
    params.K = 90.0; // Lower strike price
    double itm_call_price = models::MonteCarloModel::call_price(params);
    EXPECT_GT(itm_call_price, call_price); // ITM call should be more expensive

    // Test OTM call option
    params.K = 110.0; // Higher strike price
    double otm_call_price = models::MonteCarloModel::call_price(params);
    EXPECT_LT(otm_call_price, call_price); // OTM call should be less expensive

    // Test expired option
    params.T = 0.0;
    double expired_call_price = models::MonteCarloModel::call_price(params);
    EXPECT_DOUBLE_EQ(expired_call_price, std::max(0.0, params.S - params.K));
}

// Test put option pricing
TEST_F(MonteCarloModelTest, PutPrice) {
    // Test ATM put option
    double put_price = models::MonteCarloModel::put_price(params);
    EXPECT_GT(put_price, 0.0);
    EXPECT_LT(put_price,
              params.K); // Put price should be less than strike price

    // Test ITM put option
    params.K = 110.0; // Higher strike price
    double itm_put_price = models::MonteCarloModel::put_price(params);
    EXPECT_GT(itm_put_price, put_price); // ITM put should be more expensive

    // Test OTM put option
    params.K = 90.0; // Lower strike price
    double otm_put_price = models::MonteCarloModel::put_price(params);
    EXPECT_LT(otm_put_price, put_price); // OTM put should be less expensive

    // Test expired option
    params.T = 0.0;
    double expired_put_price = models::MonteCarloModel::put_price(params);
    EXPECT_DOUBLE_EQ(expired_put_price, std::max(0.0, params.K - params.S));
}

// Test put-call parity
TEST_F(MonteCarloModelTest, PutCallParity) {
    // Put-call parity: C - P = S - K*e^(-rT)
    double call_price = models::MonteCarloModel::call_price(params);
    double put_price = models::MonteCarloModel::put_price(params);
    double expected_diff = params.S - params.K * std::exp(-params.r * params.T);

    // Allow for some numerical error due to Monte Carlo simulation
    EXPECT_NEAR(call_price - put_price, expected_diff, 0.5);
}

// Test call delta
TEST_F(MonteCarloModelTest, CallDelta) {
    // Test ATM call delta (should be around 0.5)
    double atm_delta = models::MonteCarloModel::call_delta(params);
    EXPECT_NEAR(atm_delta, 0.5, 0.1); // Allow for some numerical error

    // Test ITM call delta (should be closer to 1.0)
    params.K = 90.0;
    double itm_delta = models::MonteCarloModel::call_delta(params);
    EXPECT_GT(itm_delta, atm_delta);
    EXPECT_NEAR(itm_delta, 0.7, 0.1); // Allow for some numerical error

    // Test OTM call delta (should be closer to 0.0)
    params.K = 110.0;
    double otm_delta = models::MonteCarloModel::call_delta(params);
    EXPECT_LT(otm_delta, atm_delta);
    EXPECT_NEAR(otm_delta, 0.3, 0.1); // Allow for some numerical error
}

// Test put delta
TEST_F(MonteCarloModelTest, PutDelta) {
    // Test ATM put delta (should be around -0.5)
    double atm_delta = models::MonteCarloModel::put_delta(params);
    EXPECT_NEAR(atm_delta, -0.5, 0.1); // Allow for some numerical error

    // Test ITM put delta (should be closer to -1.0)
    params.K = 110.0;
    double itm_delta = models::MonteCarloModel::put_delta(params);
    EXPECT_LT(itm_delta, atm_delta);
    EXPECT_NEAR(itm_delta, -0.7, 0.1); // Allow for some numerical error

    // Test OTM put delta (should be closer to 0.0)
    params.K = 90.0;
    double otm_delta = models::MonteCarloModel::put_delta(params);
    EXPECT_GT(otm_delta, atm_delta);
    EXPECT_NEAR(otm_delta, -0.3, 0.1); // Allow for some numerical error
}

// Test gamma
TEST_F(MonteCarloModelTest, Gamma) {
    // Gamma should be positive and highest for ATM options
    double atm_gamma = models::MonteCarloModel::gamma(params);
    EXPECT_GT(atm_gamma, 0.0);

    // Test ITM gamma (should be lower than ATM)
    params.K = 90.0;
    double itm_gamma = models::MonteCarloModel::gamma(params);
    EXPECT_LT(itm_gamma, atm_gamma);

    // Test OTM gamma (should be lower than ATM)
    params.K = 110.0;
    double otm_gamma = models::MonteCarloModel::gamma(params);
    EXPECT_LT(otm_gamma, atm_gamma);
}

// Test vega
TEST_F(MonteCarloModelTest, Vega) {
    // Vega should be positive
    double vega = models::MonteCarloModel::vega(params);
    EXPECT_GT(vega, 0.0);

    // Vega should increase with time to expiration
    double vega_short = models::MonteCarloModel::vega(params);
    params.T = 0.5;
    double vega_long = models::MonteCarloModel::vega(params);
    EXPECT_GT(vega_long, vega_short);
}

// Test with OptionParameters struct
TEST_F(MonteCarloModelTest, OptionParametersStruct) {
    // Create an OptionParameters struct
    models::OptionParameters test_params(100.0, 100.0, 0.05, 0.2, 1.0);

    // Test call price
    double call_price = models::MonteCarloModel::call_price(test_params);
    EXPECT_GT(call_price, 0.0);

    // Test put price
    double put_price = models::MonteCarloModel::put_price(test_params);
    EXPECT_GT(put_price, 0.0);

    // Test put-call parity
    double expected_diff =
        test_params.S -
        test_params.K * std::exp(-test_params.r * test_params.T);
    EXPECT_NEAR(call_price - put_price, expected_diff, 0.5);
}

} // namespace tests
} // namespace thales
