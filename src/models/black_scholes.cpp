#include <thales/models/black_scholes.h>
#include <cmath>
#include <stdexcept>

namespace thales {
namespace models {

double BlackScholes::callPrice(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, return intrinsic value
        return std::max(0.0, S - K);
    }
    
    double d1Val = d1(S, K, r, sigma, T);
    double d2Val = d2(S, K, r, sigma, T);
    
    return S * normalCDF(d1Val) - K * std::exp(-r * T) * normalCDF(d2Val);
}

double BlackScholes::putPrice(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, return intrinsic value
        return std::max(0.0, K - S);
    }
    
    double d1Val = d1(S, K, r, sigma, T);
    double d2Val = d2(S, K, r, sigma, T);
    
    return K * std::exp(-r * T) * normalCDF(-d2Val) - S * normalCDF(-d1Val);
}

double BlackScholes::callDelta(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, delta is either 0 or 1
        return (S > K) ? 1.0 : 0.0;
    }
    
    return normalCDF(d1(S, K, r, sigma, T));
}

double BlackScholes::putDelta(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, delta is either -1 or 0
        return (S < K) ? -1.0 : 0.0;
    }
    
    return normalCDF(d1(S, K, r, sigma, T)) - 1.0;
}

double BlackScholes::gamma(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, gamma is 0
        return 0.0;
    }
    
    double d1Val = d1(S, K, r, sigma, T);
    
    return normalPDF(d1Val) / (S * sigma * std::sqrt(T));
}

double BlackScholes::vega(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, vega is 0
        return 0.0;
    }
    
    double d1Val = d1(S, K, r, sigma, T);
    
    return S * std::sqrt(T) * normalPDF(d1Val) / 100.0; // Divided by 100 to get the change per 1% change in volatility
}

double BlackScholes::callTheta(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, theta is 0
        return 0.0;
    }
    
    double d1Val = d1(S, K, r, sigma, T);
    double d2Val = d2(S, K, r, sigma, T);
    
    double term1 = -S * sigma * normalPDF(d1Val) / (2.0 * std::sqrt(T));
    double term2 = -r * K * std::exp(-r * T) * normalCDF(d2Val);
    
    return (term1 + term2) / 365.0; // Divided by 365 to get the daily theta
}

double BlackScholes::putTheta(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, theta is 0
        return 0.0;
    }
    
    double d1Val = d1(S, K, r, sigma, T);
    double d2Val = d2(S, K, r, sigma, T);
    
    double term1 = -S * sigma * normalPDF(d1Val) / (2.0 * std::sqrt(T));
    double term2 = r * K * std::exp(-r * T) * normalCDF(-d2Val);
    
    return (term1 + term2) / 365.0; // Divided by 365 to get the daily theta
}

double BlackScholes::callRho(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, rho is 0
        return 0.0;
    }
    
    double d2Val = d2(S, K, r, sigma, T);
    
    return K * T * std::exp(-r * T) * normalCDF(d2Val) / 100.0; // Divided by 100 to get the change per 1% change in interest rate
}

double BlackScholes::putRho(double S, double K, double r, double sigma, double T) {
    if (T <= 0) {
        // For expired options, rho is 0
        return 0.0;
    }
    
    double d2Val = d2(S, K, r, sigma, T);
    
    return -K * T * std::exp(-r * T) * normalCDF(-d2Val) / 100.0; // Divided by 100 to get the change per 1% change in interest rate
}

double BlackScholes::callImpliedVolatility(
    double price, double S, double K, double r, double T,
    double epsilon, int maxIterations
) {
    if (T <= 0) {
        throw std::invalid_argument("Time to expiration must be positive");
    }
    
    // Check for arbitrage bounds
    double intrinsicValue = std::max(0.0, S - K * std::exp(-r * T));
    if (price < intrinsicValue) {
        throw std::invalid_argument("Option price violates arbitrage bounds");
    }
    
    // Initial guess for implied volatility
    double sigma = 0.2; // Start with 20% volatility
    
    // Newton-Raphson method
    for (int i = 0; i < maxIterations; ++i) {
        double priceEstimate = callPrice(S, K, r, sigma, T);
        double diff = price - priceEstimate;
        
        if (std::abs(diff) < epsilon) {
            return sigma;
        }
        
        double vegaEstimate = vega(S, K, r, sigma, T);
        if (vegaEstimate == 0.0) {
            throw std::runtime_error("Vega is zero, cannot calculate implied volatility");
        }
        
        sigma += diff / (vegaEstimate * 100.0); // Multiply by 100 because vega is per 1% change
        
        // Ensure sigma stays within reasonable bounds
        if (sigma <= 0.001) {
            sigma = 0.001;
        } else if (sigma > 10.0) {
            throw std::runtime_error("Implied volatility calculation did not converge");
        }
    }
    
    throw std::runtime_error("Implied volatility calculation did not converge within maximum iterations");
}

double BlackScholes::putImpliedVolatility(
    double price, double S, double K, double r, double T,
    double epsilon, int maxIterations
) {
    if (T <= 0) {
        throw std::invalid_argument("Time to expiration must be positive");
    }
    
    // Check for arbitrage bounds
    double intrinsicValue = std::max(0.0, K * std::exp(-r * T) - S);
    if (price < intrinsicValue) {
        throw std::invalid_argument("Option price violates arbitrage bounds");
    }
    
    // Initial guess for implied volatility
    double sigma = 0.2; // Start with 20% volatility
    
    // Newton-Raphson method
    for (int i = 0; i < maxIterations; ++i) {
        double priceEstimate = putPrice(S, K, r, sigma, T);
        double diff = price - priceEstimate;
        
        if (std::abs(diff) < epsilon) {
            return sigma;
        }
        
        double vegaEstimate = vega(S, K, r, sigma, T);
        if (vegaEstimate == 0.0) {
            throw std::runtime_error("Vega is zero, cannot calculate implied volatility");
        }
        
        sigma += diff / (vegaEstimate * 100.0); // Multiply by 100 because vega is per 1% change
        
        // Ensure sigma stays within reasonable bounds
        if (sigma <= 0.001) {
            sigma = 0.001;
        } else if (sigma > 10.0) {
            throw std::runtime_error("Implied volatility calculation did not converge");
        }
    }
    
    throw std::runtime_error("Implied volatility calculation did not converge within maximum iterations");
}

double BlackScholes::d1(double S, double K, double r, double sigma, double T) {
    return (std::log(S / K) + (r + sigma * sigma / 2.0) * T) / (sigma * std::sqrt(T));
}

double BlackScholes::d2(double S, double K, double r, double sigma, double T) {
    return d1(S, K, r, sigma, T) - sigma * std::sqrt(T);
}

double BlackScholes::normalCDF(double x) {
    // Approximation of the cumulative distribution function of the standard normal distribution
    // This is the Abramowitz and Stegun approximation
    
    const double b1 =  0.31938153;
    const double b2 = -0.356563782;
    const double b3 =  1.781477937;
    const double b4 = -1.821255978;
    const double b5 =  1.330274429;
    const double p  =  0.2316419;
    const double c  =  0.39894228;
    
    if (x >= 0.0) {
        double t = 1.0 / (1.0 + p * x);
        return 1.0 - c * std::exp(-x * x / 2.0) * t * (t * (t * (t * (t * b5 + b4) + b3) + b2) + b1);
    } else {
        double t = 1.0 / (1.0 - p * x);
        return c * std::exp(-x * x / 2.0) * t * (t * (t * (t * (t * b5 + b4) + b3) + b2) + b1);
    }
}

double BlackScholes::normalPDF(double x) {
    // Probability density function of the standard normal distribution
    return (1.0 / std::sqrt(2.0 * M_PI)) * std::exp(-0.5 * x * x);
}

} // namespace models
} // namespace thales
