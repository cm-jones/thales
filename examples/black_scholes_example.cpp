#include <iostream>
#include <iomanip>
#include <thales/models/black_scholes.h>

using namespace thales::models;

int main() {
    // Option parameters
    double S = 100.0;  // Current stock price
    double K = 100.0;  // Strike price
    double r = 0.05;   // Risk-free rate (5%)
    double sigma = 0.2; // Volatility (20%)
    double T = 1.0;    // Time to expiration (1 year)
    
    // Calculate option prices
    double callPrice = BlackScholes::callPrice(S, K, r, sigma, T);
    double putPrice = BlackScholes::putPrice(S, K, r, sigma, T);
    
    // Calculate Greeks
    double callDelta = BlackScholes::callDelta(S, K, r, sigma, T);
    double putDelta = BlackScholes::putDelta(S, K, r, sigma, T);
    double gamma = BlackScholes::gamma(S, K, r, sigma, T);
    double vega = BlackScholes::vega(S, K, r, sigma, T);
    double callTheta = BlackScholes::callTheta(S, K, r, sigma, T);
    double putTheta = BlackScholes::putTheta(S, K, r, sigma, T);
    double callRho = BlackScholes::callRho(S, K, r, sigma, T);
    double putRho = BlackScholes::putRho(S, K, r, sigma, T);
    
    // Print results
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Black-Scholes Option Pricing Example" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Option Parameters:" << std::endl;
    std::cout << "  Stock Price (S): $" << S << std::endl;
    std::cout << "  Strike Price (K): $" << K << std::endl;
    std::cout << "  Risk-free Rate (r): " << r * 100 << "%" << std::endl;
    std::cout << "  Volatility (sigma): " << sigma * 100 << "%" << std::endl;
    std::cout << "  Time to Expiration (T): " << T << " years" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Option Prices:" << std::endl;
    std::cout << "  Call Price: $" << callPrice << std::endl;
    std::cout << "  Put Price: $" << putPrice << std::endl;
    std::cout << std::endl;
    
    std::cout << "Option Greeks:" << std::endl;
    std::cout << "  Call Delta: " << callDelta << std::endl;
    std::cout << "  Put Delta: " << putDelta << std::endl;
    std::cout << "  Gamma: " << gamma << std::endl;
    std::cout << "  Vega: " << vega << " (per 1% change in volatility)" << std::endl;
    std::cout << "  Call Theta: " << callTheta << " (per day)" << std::endl;
    std::cout << "  Put Theta: " << putTheta << " (per day)" << std::endl;
    std::cout << "  Call Rho: " << callRho << " (per 1% change in interest rate)" << std::endl;
    std::cout << "  Put Rho: " << putRho << " (per 1% change in interest rate)" << std::endl;
    std::cout << std::endl;
    
    // Verify put-call parity
    double leftSide = callPrice - putPrice;
    double rightSide = S - K * std::exp(-r * T);
    
    std::cout << "Put-Call Parity Check:" << std::endl;
    std::cout << "  C - P = " << leftSide << std::endl;
    std::cout << "  S - K*e^(-rT) = " << rightSide << std::endl;
    std::cout << "  Difference: " << std::abs(leftSide - rightSide) << std::endl;
    std::cout << std::endl;
    
    // Calculate implied volatility
    double impliedVolCall = BlackScholes::callImpliedVolatility(callPrice, S, K, r, T);
    double impliedVolPut = BlackScholes::putImpliedVolatility(putPrice, S, K, r, T);
    
    std::cout << "Implied Volatility:" << std::endl;
    std::cout << "  From Call Price: " << impliedVolCall * 100 << "%" << std::endl;
    std::cout << "  From Put Price: " << impliedVolPut * 100 << "%" << std::endl;
    std::cout << std::endl;
    
    // Demonstrate the effect of changing volatility
    std::cout << "Effect of Changing Volatility on Option Prices:" << std::endl;
    std::cout << "  Volatility | Call Price | Put Price" << std::endl;
    std::cout << "  -----------|------------|----------" << std::endl;
    
    for (double vol = 0.1; vol <= 0.5; vol += 0.1) {
        double callPriceVol = BlackScholes::callPrice(S, K, r, vol, T);
        double putPriceVol = BlackScholes::putPrice(S, K, r, vol, T);
        std::cout << "  " << std::setw(9) << vol * 100 << "% | $" 
                  << std::setw(9) << callPriceVol << " | $" 
                  << std::setw(8) << putPriceVol << std::endl;
    }
    std::cout << std::endl;
    
    // Demonstrate the effect of changing time to expiration
    std::cout << "Effect of Changing Time to Expiration on Option Prices:" << std::endl;
    std::cout << "  Time (years) | Call Price | Put Price" << std::endl;
    std::cout << "  -------------|------------|----------" << std::endl;
    
    for (double time = 0.25; time <= 2.0; time += 0.25) {
        double callPriceTime = BlackScholes::callPrice(S, K, r, sigma, time);
        double putPriceTime = BlackScholes::putPrice(S, K, r, sigma, time);
        std::cout << "  " << std::setw(11) << time << " | $" 
                  << std::setw(9) << callPriceTime << " | $" 
                  << std::setw(8) << putPriceTime << std::endl;
    }
    
    return 0;
}
