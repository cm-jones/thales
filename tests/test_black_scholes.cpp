#include <gtest/gtest.h>
#include <thales/models/black_scholes.h>
#include <cmath>

using namespace thales::models;

// Helper function to compare floating point values with a tolerance
bool isClose(double a, double b, double tolerance = 0.0001) {
    return std::abs(a - b) < tolerance;
}

TEST(BlackScholesTest, CallPriceTest) {
    // Test case 1: At-the-money call option
    double S = 100.0;  // Current stock price
    double K = 100.0;  // Strike price
    double r = 0.05;   // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;    // Time to expiration (1 year)
    
    double callPrice = BlackScholes::callPrice(S, K, r, sigma, T);
    
    // Expected price calculated using a reference implementation
    double expectedPrice = 10.45;
    
    EXPECT_TRUE(isClose(callPrice, expectedPrice, 0.01));
    
    // Test case 2: In-the-money call option
    S = 110.0;
    K = 100.0;
    
    callPrice = BlackScholes::callPrice(S, K, r, sigma, T);
    expectedPrice = 18.10;
    
    EXPECT_TRUE(isClose(callPrice, expectedPrice, 0.01));
    
    // Test case 3: Out-of-the-money call option
    S = 90.0;
    K = 100.0;
    
    callPrice = BlackScholes::callPrice(S, K, r, sigma, T);
    expectedPrice = 4.78;
    
    EXPECT_TRUE(isClose(callPrice, expectedPrice, 0.01));
}

TEST(BlackScholesTest, PutPriceTest) {
    // Test case 1: At-the-money put option
    double S = 100.0;  // Current stock price
    double K = 100.0;  // Strike price
    double r = 0.05;   // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;    // Time to expiration (1 year)
    
    double putPrice = BlackScholes::putPrice(S, K, r, sigma, T);
    
    // Expected price calculated using a reference implementation
    double expectedPrice = 5.57;
    
    EXPECT_TRUE(isClose(putPrice, expectedPrice, 0.01));
    
    // Test case 2: In-the-money put option
    S = 90.0;
    K = 100.0;
    
    putPrice = BlackScholes::putPrice(S, K, r, sigma, T);
    expectedPrice = 9.89;
    
    EXPECT_TRUE(isClose(putPrice, expectedPrice, 0.01));
    
    // Test case 3: Out-of-the-money put option
    S = 110.0;
    K = 100.0;
    
    putPrice = BlackScholes::putPrice(S, K, r, sigma, T);
    expectedPrice = 2.34;
    
    EXPECT_TRUE(isClose(putPrice, expectedPrice, 0.01));
}

TEST(BlackScholesTest, GreeksTest) {
    double S = 100.0;  // Current stock price
    double K = 100.0;  // Strike price
    double r = 0.05;   // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;    // Time to expiration (1 year)
    
    // Test Delta
    double callDelta = BlackScholes::callDelta(S, K, r, sigma, T);
    double putDelta = BlackScholes::putDelta(S, K, r, sigma, T);
    
    // Expected values
    double expectedCallDelta = 0.63;
    double expectedPutDelta = -0.37;
    
    EXPECT_TRUE(isClose(callDelta, expectedCallDelta, 0.01));
    EXPECT_TRUE(isClose(putDelta, expectedPutDelta, 0.01));
    
    // Test Gamma (same for call and put)
    double gamma = BlackScholes::gamma(S, K, r, sigma, T);
    double expectedGamma = 0.019;
    
    EXPECT_TRUE(isClose(gamma, expectedGamma, 0.001));
    
    // Test Vega (same for call and put, but scaled by 0.01)
    double vega = BlackScholes::vega(S, K, r, sigma, T);
    double expectedVega = 0.39;
    
    EXPECT_TRUE(isClose(vega, expectedVega, 0.01));
    
    // Test Theta
    double callTheta = BlackScholes::callTheta(S, K, r, sigma, T);
    double putTheta = BlackScholes::putTheta(S, K, r, sigma, T);
    
    // Expected values (daily theta)
    double expectedCallTheta = -0.05;
    double expectedPutTheta = -0.03;
    
    EXPECT_TRUE(isClose(callTheta, expectedCallTheta, 0.01));
    EXPECT_TRUE(isClose(putTheta, expectedPutTheta, 0.01));
    
    // Test Rho
    double callRho = BlackScholes::callRho(S, K, r, sigma, T);
    double putRho = BlackScholes::putRho(S, K, r, sigma, T);
    
    // Expected values (scaled by 0.01)
    double expectedCallRho = 0.50;
    double expectedPutRho = -0.44;
    
    EXPECT_TRUE(isClose(callRho, expectedCallRho, 0.01));
    EXPECT_TRUE(isClose(putRho, expectedPutRho, 0.01));
}

TEST(BlackScholesTest, ImpliedVolatilityTest) {
    double S = 100.0;  // Current stock price
    double K = 100.0;  // Strike price
    double r = 0.05;   // Risk-free rate (5%)
    double T = 1.0;    // Time to expiration (1 year)
    
    // Test call implied volatility
    double sigma = 0.2; // True volatility (20%)
    double callPrice = BlackScholes::callPrice(S, K, r, sigma, T);
    
    double impliedVol = BlackScholes::callImpliedVolatility(callPrice, S, K, r, T);
    
    EXPECT_TRUE(isClose(impliedVol, sigma, 0.0001));
    
    // Test put implied volatility
    double putPrice = BlackScholes::putPrice(S, K, r, sigma, T);
    
    impliedVol = BlackScholes::putImpliedVolatility(putPrice, S, K, r, T);
    
    EXPECT_TRUE(isClose(impliedVol, sigma, 0.0001));
}

TEST(BlackScholesTest, PutCallParityTest) {
    double S = 100.0;  // Current stock price
    double K = 100.0;  // Strike price
    double r = 0.05;   // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;    // Time to expiration (1 year)
    
    double callPrice = BlackScholes::callPrice(S, K, r, sigma, T);
    double putPrice = BlackScholes::putPrice(S, K, r, sigma, T);
    
    // Put-Call Parity: C - P = S - K * exp(-r * T)
    double leftSide = callPrice - putPrice;
    double rightSide = S - K * std::exp(-r * T);
    
    EXPECT_TRUE(isClose(leftSide, rightSide, 0.0001));
}

TEST(BlackScholesTest, ExpiredOptionsTest) {
    double S = 100.0;  // Current stock price
    double K = 100.0;  // Strike price
    double r = 0.05;   // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 0.0;    // Time to expiration (expired)
    
    // For expired options, the price should be the intrinsic value
    
    // At-the-money call (intrinsic value = 0)
    double callPrice = BlackScholes::callPrice(S, K, r, sigma, T);
    EXPECT_DOUBLE_EQ(callPrice, 0.0);
    
    // At-the-money put (intrinsic value = 0)
    double putPrice = BlackScholes::putPrice(S, K, r, sigma, T);
    EXPECT_DOUBLE_EQ(putPrice, 0.0);
    
    // In-the-money call (intrinsic value = S - K)
    S = 110.0;
    K = 100.0;
    callPrice = BlackScholes::callPrice(S, K, r, sigma, T);
    EXPECT_DOUBLE_EQ(callPrice, 10.0);
    
    // In-the-money put (intrinsic value = K - S)
    S = 90.0;
    K = 100.0;
    putPrice = BlackScholes::putPrice(S, K, r, sigma, T);
    EXPECT_DOUBLE_EQ(putPrice, 10.0);
}
