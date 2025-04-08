import pandas as pd
import numpy as np
from datetime import datetime, timedelta
from typing import Dict, List, Optional, Union, Any
import logging

from thales_backtester.models.black_scholes import BlackScholesModel


class Signal:
    """Python equivalent of the C++ Signal struct."""
    
    class Type:
        BUY = "BUY"
        SELL = "SELL"
        HOLD = "HOLD"
    
    def __init__(self, 
                symbol: str = "",
                type: str = Type.HOLD,
                strength: float = 0.0,
                target_price: float = 0.0,
                stop_loss: float = 0.0,
                timestamp: str = "",
                strategy_name: str = ""):
        self.symbol = symbol
        self.type = type
        self.strength = strength
        self.target_price = target_price
        self.stop_loss = stop_loss
        self.timestamp = timestamp
        self.strategy_name = strategy_name


class StrategyBase:
    """Base class for all trading strategies."""
    
    def __init__(self, name: str, config: Dict[str, Any]):
        self.name = name
        self.description = ""
        self.config = config
        self.symbols = []
    
    def initialize(self) -> bool:
        """Initialize the strategy."""
        raise NotImplementedError("Subclasses must implement initialize()")
    
    def execute(self, market_data: pd.DataFrame, portfolio: Dict[str, Any]) -> List[Signal]:
        """Execute the strategy."""
        raise NotImplementedError("Subclasses must implement execute()")
    
    def get_name(self) -> str:
        """Get the name of the strategy."""
        return self.name
    
    def get_description(self) -> str:
        """Get the description of the strategy."""
        return self.description
    
    def set_description(self, description: str):
        """Set the description of the strategy."""
        self.description = description
    
    def get_config(self) -> Dict[str, Any]:
        """Get the configuration of the strategy."""
        return self.config
    
    def set_config_value(self, key: str, value: Any):
        """Set a configuration value."""
        self.config[key] = value
    
    def get_symbols(self) -> List[str]:
        """Get the symbols that the strategy is interested in."""
        return self.symbols


class BlackScholesArbitrage(StrategyBase):
    """
    Strategy based on Black-Scholes pricing model.
    
    This strategy compares market prices of options with their theoretical prices
    calculated using the Black-Scholes model. If the market price is significantly
    different from the theoretical price, it generates a trading signal.
    """
    
    def __init__(self, config: Dict[str, Any]):
        super().__init__("BlackScholesArbitrage", config)
        
        # Default strategy parameters
        self.min_price_discrepancy = config.get("min_price_discrepancy", 0.10)  # 10%
        self.min_volatility = config.get("min_volatility", 0.10)  # 10%
        self.max_volatility = config.get("max_volatility", 0.50)  # 50%
        self.min_days_to_expiration = config.get("min_days_to_expiration", 7)
        self.max_days_to_expiration = config.get("max_days_to_expiration", 45)
        
        # Symbols to trade
        self.symbols = config.get("symbols", [])
        
        # Historical volatility cache
        self.historical_volatility = {}
        
        # Logger
        self.logger = logging.getLogger("BlackScholesArbitrage")
        self.logger.setLevel(logging.INFO)
    
    def initialize(self) -> bool:
        """Initialize the strategy."""
        # Validate the strategy configuration
        if not self.symbols:
            self.logger.warning("No symbols configured")
            return False
        
        if self.min_price_discrepancy <= 0:
            self.logger.warning("min_price_discrepancy must be positive")
            return False
        
        if self.min_volatility <= 0 or self.max_volatility <= 0:
            self.logger.warning("Volatility bounds must be positive")
            return False
        
        if self.min_days_to_expiration <= 0 or self.max_days_to_expiration <= 0:
            self.logger.warning("Days to expiration bounds must be positive")
            return False
        
        if self.min_days_to_expiration > self.max_days_to_expiration:
            self.logger.warning("min_days_to_expiration must be <= max_days_to_expiration")
            return False
        
        self.logger.info(f"Initialized BlackScholesArbitrage strategy with {len(self.symbols)} symbols")
        return True
    
    def execute(self, market_data: pd.DataFrame, portfolio: Dict[str, Any]) -> List[Signal]:
        """
        Execute the Black-Scholes arbitrage strategy.
        
        Args:
            market_data: DataFrame containing option data
            portfolio: Dictionary containing portfolio information
            
        Returns:
            List of trading signals
        """
        signals = []
        
        # Skip if no options data
        if market_data.empty:
            return signals
        
        # Filter out non-option data
        options_data = market_data[market_data['option_type'].notna()]
        
        # Skip if no options data available
        if options_data.empty:
            return signals
        
        # Group by symbol to process each symbol separately
        for symbol, symbol_data in options_data.groupby('symbol'):
            # Skip if not in our configured symbols
            if self.symbols and symbol not in self.symbols:
                continue
            
            # Update historical volatility for this symbol
            self._update_historical_volatility(symbol, market_data)
            
            # Process each option
            for _, option in symbol_data.iterrows():
                # Skip if option doesn't meet our criteria
                if not self._is_option_eligible(option):
                    continue
                
                # Calculate theoretical price
                theoretical_price = self._calculate_theoretical_price(option)
                
                # Skip if we couldn't calculate a valid price
                if np.isnan(theoretical_price):
                    continue
                
                # Calculate price discrepancy
                market_price = option['mid']
                discrepancy_pct = abs(market_price - theoretical_price) / theoretical_price
                
                # Generate signal if discrepancy is significant
                if discrepancy_pct >= self.min_price_discrepancy:
                    signal = self._generate_signal(option, theoretical_price)
                    signals.append(signal)
        
        return signals
    
    def _update_historical_volatility(self, symbol: str, market_data: pd.DataFrame):
        """
        Update historical volatility for a symbol.
        
        Args:
            symbol: Symbol to update
            market_data: DataFrame containing market data
        """
        # Filter for this symbol and extract underlying prices
        symbol_data = market_data[market_data['symbol'] == symbol]
        
        if symbol_data.empty:
            return
        
        # Get the unique dates
        dates = pd.to_datetime(symbol_data['date'].unique())
        
        # If we have at least 2 days of data, calculate historical volatility
        if len(dates) >= 2:
            # Get underlying prices for each day
            prices = []
            for date in sorted(dates):
                date_data = symbol_data[symbol_data['date'] == date]
                
                if 'underlying_price' in date_data.columns:
                    price = date_data['underlying_price'].iloc[0]
                    prices.append(price)
            
            if len(prices) >= 2:
                # Calculate returns
                returns = np.diff(np.log(prices))
                
                # Calculate annualized volatility
                daily_vol = np.std(returns)
                annualized_vol = daily_vol * np.sqrt(252)  # Assuming 252 trading days
                
                # Store in cache
                self.historical_volatility[symbol] = annualized_vol
    
    def _is_option_eligible(self, option) -> bool:
        """
        Check if an option meets the criteria for this strategy.
        
        Args:
            option: Option data
            
        Returns:
            True if eligible, False otherwise
        """
        # Check if we have all required data
        for field in ['option_type', 'strike', 'expiration', 'date', 'mid', 'underlying_price']:
            if field not in option or pd.isna(option[field]):
                return False
        
        # Check option type (only handle calls and puts)
        option_type = str(option['option_type']).lower()
        if option_type not in ['call', 'put']:
            return False
        
        # Check expiration range
        if 'days_to_expiry' in option:
            days_to_expiry = option['days_to_expiry']
        else:
            # Calculate days to expiration
            option_date = pd.to_datetime(option['date'])
            expiration = pd.to_datetime(option['expiration'])
            days_to_expiry = (expiration - option_date).days
        
        if days_to_expiry < self.min_days_to_expiration or days_to_expiry > self.max_days_to_expiration:
            return False
        
        # Check volatility
        symbol = option['symbol']
        if symbol in self.historical_volatility:
            vol = self.historical_volatility[symbol]
            if vol < self.min_volatility or vol > self.max_volatility:
                return False
        
        # Check if mid price is valid
        if option['mid'] <= 0:
            return False
        
        return True
    
    def _calculate_theoretical_price(self, option) -> float:
        """
        Calculate the theoretical price of an option using the Black-Scholes model.
        
        Args:
            option: Option data
            
        Returns:
            Theoretical price
        """
        # Extract parameters
        S = option['underlying_price']  # Current stock price
        K = option['strike']  # Strike price
        
        # Calculate time to expiration in years
        if 'days_to_expiry' in option:
            T = option['days_to_expiry'] / 365.0
        else:
            option_date = pd.to_datetime(option['date'])
            expiration = pd.to_datetime(option['expiration'])
            T = (expiration - option_date).days / 365.0
        
        # Use historical volatility or default
        symbol = option['symbol']
        if symbol in self.historical_volatility:
            sigma = self.historical_volatility[symbol]
        else:
            sigma = 0.30  # Default volatility: 30%
        
        # Risk-free rate (simplified) - normally this would come from market data
        r = 0.02  # 2%
        
        # Calculate theoretical price
        option_type = str(option['option_type']).lower()
        
        if option_type == 'call':
            return BlackScholesModel.call_price(S, K, r, sigma, T)
        elif option_type == 'put':
            return BlackScholesModel.put_price(S, K, r, sigma, T)
        else:
            return float('nan')
    
    def _generate_signal(self, option, theoretical_price: float) -> Signal:
        """
        Generate a trading signal for an option.
        
        Args:
            option: Option data
            theoretical_price: Theoretical price from Black-Scholes model
            
        Returns:
            Trading signal
        """
        market_price = option['mid']
        
        # Determine signal type based on price discrepancy
        if market_price < theoretical_price:
            # Option is underpriced - buy signal
            signal_type = Signal.Type.BUY
            signal_strength = (theoretical_price - market_price) / theoretical_price
        else:
            # Option is overpriced - sell signal
            signal_type = Signal.Type.SELL
            signal_strength = (market_price - theoretical_price) / theoretical_price
        
        # Construct the option symbol
        option_type = str(option['option_type']).lower()
        expiration = pd.to_datetime(option['expiration']).strftime('%Y%m%d')
        strike = f"{option['strike']:.2f}"
        option_symbol = f"{option['symbol']}_{expiration}_{option_type}_{strike}"
        
        # Generate signal
        signal = Signal(
            symbol=option_symbol,
            type=signal_type,
            strength=min(signal_strength, 1.0),  # Cap strength at 1.0
            target_price=theoretical_price,
            stop_loss=market_price * 0.9 if signal_type == Signal.Type.BUY else market_price * 1.1,
            timestamp=pd.to_datetime(option['date']).strftime('%Y-%m-%d %H:%M:%S'),
            strategy_name=self.get_name()
        )
        
        return signal
