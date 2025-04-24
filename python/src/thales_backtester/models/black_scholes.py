import numpy as np
from scipy.stats import norm
import math
from typing import Dict, Literal, Optional, Tuple, Union


class BlackScholesModel:
    """
    Python implementation of the Black-Scholes-Merton option pricing model.
    This serves as a fallback when the C++ bindings are not available.
    """
    
    @staticmethod
    def d1(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the d1 parameter for Black-Scholes formula.
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            d1 parameter value
        """
        if T <= 0 or sigma <= 0:
            # Handle edge cases to avoid math domain errors
            return float('nan')
            
        return (np.log(S / K) + (r + 0.5 * sigma**2) * T) / (sigma * np.sqrt(T))
    
    @staticmethod
    def d2(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the d2 parameter for Black-Scholes formula.
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            d2 parameter value
        """
        if T <= 0 or sigma <= 0:
            # Handle edge cases to avoid math domain errors
            return float('nan')
            
        return BlackScholesModel.d1(S, K, r, sigma, T) - sigma * np.sqrt(T)
    
    @staticmethod
    def call_price(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the theoretical price of a European call option.
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Theoretical call option price
        """
        if T <= 0:  # Handle expired options
            return max(0, S - K)
            
        if sigma <= 0:  # Handle zero volatility
            return max(0, S - K * np.exp(-r * T))
            
        d1_val = BlackScholesModel.d1(S, K, r, sigma, T)
        d2_val = BlackScholesModel.d2(S, K, r, sigma, T)
        
        if np.isnan(d1_val) or np.isnan(d2_val):
            return float('nan')
            
        return S * norm.cdf(d1_val) - K * np.exp(-r * T) * norm.cdf(d2_val)
    
    @staticmethod
    def put_price(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the theoretical price of a European put option.
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Theoretical put option price
        """
        if T <= 0:  # Handle expired options
            return max(0, K - S)
            
        if sigma <= 0:  # Handle zero volatility
            return max(0, K * np.exp(-r * T) - S)
            
        d1_val = BlackScholesModel.d1(S, K, r, sigma, T)
        d2_val = BlackScholesModel.d2(S, K, r, sigma, T)
        
        if np.isnan(d1_val) or np.isnan(d2_val):
            return float('nan')
            
        return K * np.exp(-r * T) * norm.cdf(-d2_val) - S * norm.cdf(-d1_val)
    
    @staticmethod
    def call_delta(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the delta of a call option.
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Call option delta
        """
        if T <= 0:  # Handle expired options
            return 1.0 if S > K else 0.0
            
        d1_val = BlackScholesModel.d1(S, K, r, sigma, T)
        
        if np.isnan(d1_val):
            return float('nan')
            
        return norm.cdf(d1_val)
    
    @staticmethod
    def put_delta(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the delta of a put option.
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Put option delta
        """
        if T <= 0:  # Handle expired options
            return -1.0 if S < K else 0.0
            
        d1_val = BlackScholesModel.d1(S, K, r, sigma, T)
        
        if np.isnan(d1_val):
            return float('nan')
            
        return norm.cdf(d1_val) - 1.0
    
    @staticmethod
    def gamma(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the gamma of an option (same for calls and puts).
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Option gamma
        """
        if T <= 0 or sigma <= 0:  # Handle edge cases
            return 0.0
            
        d1_val = BlackScholesModel.d1(S, K, r, sigma, T)
        
        if np.isnan(d1_val):
            return float('nan')
            
        return norm.pdf(d1_val) / (S * sigma * np.sqrt(T))
    
    @staticmethod
    def vega(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the vega of an option.
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Option vega (per 1% change in volatility)
        """
        if T <= 0 or sigma <= 0:  # Handle edge cases
            return 0.0
            
        d1_val = BlackScholesModel.d1(S, K, r, sigma, T)
        
        if np.isnan(d1_val):
            return float('nan')
            
        # Scale by 0.01 to get the change for a 1% change in volatility
        return S * np.sqrt(T) * norm.pdf(d1_val) * 0.01
    
    @staticmethod
    def call_theta(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the theta of a call option (time decay).
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Call option theta (per year)
        """
        if T <= 0 or sigma <= 0:  # Handle edge cases
            return 0.0
            
        d1_val = BlackScholesModel.d1(S, K, r, sigma, T)
        d2_val = BlackScholesModel.d2(S, K, r, sigma, T)
        
        if np.isnan(d1_val) or np.isnan(d2_val):
            return float('nan')
            
        common_term = -(S * sigma * norm.pdf(d1_val)) / (2 * np.sqrt(T))
        specific_term = -r * K * np.exp(-r * T) * norm.cdf(d2_val)
        
        # Return daily theta (divided by 365)
        return (common_term + specific_term) / 365.0
    
    @staticmethod
    def put_theta(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the theta of a put option (time decay).
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Put option theta (per year)
        """
        if T <= 0 or sigma <= 0:  # Handle edge cases
            return 0.0
            
        d1_val = BlackScholesModel.d1(S, K, r, sigma, T)
        d2_val = BlackScholesModel.d2(S, K, r, sigma, T)
        
        if np.isnan(d1_val) or np.isnan(d2_val):
            return float('nan')
            
        common_term = -(S * sigma * norm.pdf(d1_val)) / (2 * np.sqrt(T))
        specific_term = r * K * np.exp(-r * T) * norm.cdf(-d2_val)
        
        # Return daily theta (divided by 365)
        return (common_term + specific_term) / 365.0
    
    @staticmethod
    def call_rho(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the rho of a call option.
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Call option rho (per 1% change in interest rate)
        """
        if T <= 0:  # Handle expired options
            return 0.0
            
        d2_val = BlackScholesModel.d2(S, K, r, sigma, T)
        
        if np.isnan(d2_val):
            return float('nan')
            
        # Scale by 0.01 to get the change for a 1% change in interest rate
        return K * T * np.exp(-r * T) * norm.cdf(d2_val) * 0.01
    
    @staticmethod
    def put_rho(S: float, K: float, r: float, sigma: float, T: float) -> float:
        """
        Calculate the rho of a put option.
        
        Args:
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            sigma: Volatility of the underlying (annualized)
            T: Time to expiration in years
            
        Returns:
            Put option rho (per 1% change in interest rate)
        """
        if T <= 0:  # Handle expired options
            return 0.0
            
        d2_val = BlackScholesModel.d2(S, K, r, sigma, T)
        
        if np.isnan(d2_val):
            return float('nan')
            
        # Scale by 0.01 to get the change for a 1% change in interest rate
        return -K * T * np.exp(-r * T) * norm.cdf(-d2_val) * 0.01
    
    @staticmethod
    def call_implied_volatility(price: float, S: float, K: float, r: float, T: float, 
                               epsilon: float = 0.0001, max_iterations: int = 100) -> float:
        """
        Calculate the implied volatility of a call option using the Newton-Raphson method.
        
        Args:
            price: Market price of the option
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            T: Time to expiration in years
            epsilon: Desired precision for implied volatility
            max_iterations: Maximum number of iterations
            
        Returns:
            Implied volatility
        """
        if T <= 0:  # Handle expired options
            return float('nan')
            
        if price <= 0:  # Handle invalid prices
            return float('nan')
            
        # Check for arbitrage bounds
        intrinsic_value = max(0, S - K * np.exp(-r * T))
        if price < intrinsic_value:
            return float('nan')  # Price violates arbitrage bounds
            
        # Initial guess for implied volatility
        sigma = 0.2  # Start with 20% volatility
        
        for i in range(max_iterations):
            price_diff = BlackScholesModel.call_price(S, K, r, sigma, T) - price
            
            if abs(price_diff) < epsilon:
                return sigma
                
            vega_val = BlackScholesModel.vega(S, K, r, sigma, T) * 100  # Adjust vega scale
            
            if abs(vega_val) < 1e-10:
                return float('nan')
                
            sigma = sigma - price_diff / vega_val
            
            # Ensure sigma stays within reasonable bounds
            if sigma <= 0.001:
                sigma = 0.001
            elif sigma > 10:  # 1000% volatility cap
                return float('nan')
                
        # Failed to converge
        return float('nan')
    
    @staticmethod
    def put_implied_volatility(price: float, S: float, K: float, r: float, T: float,
                              epsilon: float = 0.0001, max_iterations: int = 100) -> float:
        """
        Calculate the implied volatility of a put option using the Newton-Raphson method.
        
        Args:
            price: Market price of the option
            S: Current stock price
            K: Option strike price
            r: Risk-free interest rate (annualized)
            T: Time to expiration in years
            epsilon: Desired precision for implied volatility
            max_iterations: Maximum number of iterations
            
        Returns:
            Implied volatility
        """
        if T <= 0:  # Handle expired options
            return float('nan')
            
        if price <= 0:  # Handle invalid prices
            return float('nan')
            
        # Check for arbitrage bounds
        intrinsic_value = max(0, K * np.exp(-r * T) - S)
        if price < intrinsic_value:
            return float('nan')  # Price violates arbitrage bounds
            
        # Initial guess for implied volatility
        sigma = 0.2  # Start with 20% volatility
        
        for i in range(max_iterations):
            price_diff = BlackScholesModel.put_price(S, K, r, sigma, T) - price
            
            if abs(price_diff) < epsilon:
                return sigma
                
            vega_val = BlackScholesModel.vega(S, K, r, sigma, T) * 100  # Adjust vega scale
            
            if abs(vega_val) < 1e-10:
                return float('nan')
                
            sigma = sigma - price_diff / vega_val
            
            # Ensure sigma stays within reasonable bounds
            if sigma <= 0.001:
                sigma = 0.001
            elif sigma > 10:  # 1000% volatility cap
                return float('nan')
                
        # Failed to converge
        return float('nan')
