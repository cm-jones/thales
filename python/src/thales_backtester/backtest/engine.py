import pandas as pd
import numpy as np
from datetime import datetime, timedelta
import matplotlib.pyplot as plt
from typing import Dict, List, Optional, Union, Tuple, Any, Callable
import logging
from dataclasses import dataclass, field
import importlib.util
import time

# Try to import the C++ bindings
try:
    from thales_backtester.thales_cpp_bindings import models, strategies
    CPP_BINDINGS_AVAILABLE = True
except ImportError:
    CPP_BINDINGS_AVAILABLE = False
    logging.warning("C++ bindings not available. Using Python implementations instead.")


class BacktestEngine:
    """
    Main backtesting engine for options trading strategies.
    This engine can use either C++ implementations or Python implementations.
    """
    
    def __init__(self, 
                 data_path: str = None,
                 use_cpp_bindings: bool = True,
                 initial_capital: float = 100000.0,
                 commission: float = 0.0):
        """
        Initialize the backtesting engine.
        
        Args:
            data_path: Path to historical data CSV file
            use_cpp_bindings: Whether to use C++ bindings for models and strategies
            initial_capital: Initial capital for backtesting
            commission: Commission per trade (in dollars)
        """
        self.data_path = data_path
        self.use_cpp_bindings = use_cpp_bindings and CPP_BINDINGS_AVAILABLE
        self.initial_capital = initial_capital
        self.commission = commission
        
        # Data containers
        self.options_data = None
        self.underlying_data = None
        
        # Performance tracking
        self.portfolio_value = []
        self.trades = []
        self.metrics = {}
        
        # Logging
        self.logger = logging.getLogger("BacktestEngine")
        self.logger.setLevel(logging.INFO)
        
        if not CPP_BINDINGS_AVAILABLE and use_cpp_bindings:
            self.logger.warning("C++ bindings requested but not available. Using Python implementations.")
    
    def load_data(self, data_path: str = None, **kwargs):
        """
        Load historical options data.
        
        Args:
            data_path: Path to CSV file with options data
            **kwargs: Additional parameters to pass to pandas.read_csv
        """
        from thales_backtester.data.data_loader import OptionsDataLoader
        
        data_path = data_path or self.data_path
        if not data_path:
            raise ValueError("No data path provided")
            
        loader = OptionsDataLoader()
        self.options_data = loader.load_csv(data_path, **kwargs)
        
        if self.options_data is None or self.options_data.empty:
            raise ValueError(f"Failed to load data from {data_path}")
            
        self.logger.info(f"Loaded {len(self.options_data)} records from {data_path}")
        
        # Extract underlying data
        self._extract_underlying_data()
        
        return self.options_data
    
    def _extract_underlying_data(self):
        """Extract underlying price data from options data."""
        if self.options_data is None:
            return
            
        if 'underlying_price' not in self.options_data.columns or 'date' not in self.options_data.columns:
            self.logger.warning("Cannot extract underlying data: missing columns")
            return
            
        # Group by date and symbol, get the first underlying price for each day
        underlying = self.options_data[['date', 'symbol', 'underlying_price']].drop_duplicates(['date', 'symbol'])
        
        self.underlying_data = underlying.pivot(index='date', columns='symbol', values='underlying_price')
        self.logger.info(f"Extracted underlying data for {len(self.underlying_data.columns)} symbols")
    
    def run_backtest(self, 
                    strategy: Union[str, Any],
                    start_date: Union[str, datetime] = None,
                    end_date: Union[str, datetime] = None,
                    config: Dict[str, Any] = None):
        """
        Run a backtest using the specified strategy.
        
        Args:
            strategy: Strategy name (string) or strategy instance
            start_date: Start date for backtesting
            end_date: End date for backtesting
            config: Strategy configuration parameters
            
        Returns:
            Dictionary with backtest results
        """
        if self.options_data is None:
            raise ValueError("No data loaded. Call load_data() first.")
            
        # Prepare dates
        if start_date is None:
            start_date = self.options_data['date'].min()
        elif isinstance(start_date, str):
            start_date = pd.to_datetime(start_date)
            
        if end_date is None:
            end_date = self.options_data['date'].max()
        elif isinstance(end_date, str):
            end_date = pd.to_datetime(end_date)
            
        # Filter data by date
        mask = (self.options_data['date'] >= start_date) & (self.options_data['date'] <= end_date)
        backtest_data = self.options_data[mask].copy()
        
        if backtest_data.empty:
            raise ValueError(f"No data available between {start_date} and {end_date}")
        
        # Initialize strategy
        if isinstance(strategy, str):
            strategy_instance = self._create_strategy(strategy, config)
        else:
            strategy_instance = strategy
            
        # Initialize portfolio and performance tracking
        portfolio = self._initialize_portfolio()
        
        # Get unique dates in chronological order
        dates = sorted(backtest_data['date'].unique())
        
        self.logger.info(f"Running backtest from {dates[0]} to {dates[-1]}")
        start_time = time.time()
        
        # Run simulation for each day
        for date in dates:
            self._process_day(date, backtest_data, strategy_instance, portfolio)
            
        end_time = time.time()
        self.logger.info(f"Backtest completed in {end_time - start_time:.2f} seconds")
        
        # Calculate performance metrics
        self._calculate_metrics()
        
        return {
            'portfolio_history': self.portfolio_value,
            'trades': self.trades,
            'metrics': self.metrics,
            'strategy': strategy_instance
        }
    
    def _create_strategy(self, strategy_name: str, config: Dict[str, Any] = None):
        """Create a strategy instance by name."""
        config = config or {}
        
        if self.use_cpp_bindings:
            # Use C++ strategy
            if strategy_name == "BlackScholesArbitrage":
                # Convert Python config to C++ config
                cpp_config = strategies.Config()
                for key, value in config.items():
                    if isinstance(value, bool):
                        cpp_config.set(key, value)
                    elif isinstance(value, int):
                        cpp_config.set(key, value)
                    elif isinstance(value, float):
                        cpp_config.set(key, value)
                    elif isinstance(value, str):
                        cpp_config.set(key, value)
                
                return strategies.BlackScholesArbitrage(cpp_config)
            else:
                raise ValueError(f"Unknown C++ strategy: {strategy_name}")
        else:
            # Use Python strategy
            try:
                # Import dynamically from thales_backtester.strategies
                module = importlib.import_module(f"thales_backtester.strategies.{strategy_name.lower()}")
                strategy_class = getattr(module, strategy_name)
                return strategy_class(config)
            except (ImportError, AttributeError) as e:
                raise ValueError(f"Strategy {strategy_name} not found: {e}")
    
    def _initialize_portfolio(self):
        """Initialize portfolio for backtesting."""
        # If using C++ bindings, create C++ Portfolio object
        if self.use_cpp_bindings:
            portfolio = strategies.Portfolio()
        else:
            # Create Python portfolio representation
            portfolio = {
                'cash': self.initial_capital,
                'positions': {},
                'value_history': []
            }
        
        self.portfolio_value = []
        self.trades = []
        
        return portfolio
    
    def _process_day(self, date, data, strategy, portfolio):
        """Process a single day in the backtest."""
        # Get data for this date
        day_data = data[data['date'] == date]
        
        # If using C++ bindings, convert to C++ objects
        if self.use_cpp_bindings:
            # Convert pandas data to C++ market data objects
            market_data = []
            for _, row in day_data.iterrows():
                md = strategies.MarketData()
                md.symbol = row['symbol']
                md.timestamp = row['date'].strftime("%Y-%m-%d %H:%M:%S")
                md.price = row['underlying_price']
                md.volume = row.get('volume', 0)
                market_data.append(md)
            
            # Execute strategy with C++ objects
            signals = strategy.execute(market_data, portfolio)
            
            # Process signals
            for signal in signals:
                self._process_signal(signal, date, day_data, portfolio)
        else:
            # Use Python implementation
            # This would call a Python strategy's execute method
            signals = strategy.execute(day_data, portfolio)
            
            # Process signals
            for signal in signals:
                self._process_signal(signal, date, day_data, portfolio)
        
        # Update portfolio value
        portfolio_value = self._calculate_portfolio_value(portfolio, date, day_data)
        self.portfolio_value.append({
            'date': date,
            'value': portfolio_value
        })
    
    def _process_signal(self, signal, date, data, portfolio):
        """Process a trading signal."""
        # This is a simplified implementation
        # In a real system, this would handle order execution, position management, etc.
        
        if self.use_cpp_bindings:
            # Process C++ signal
            signal_type = signal.type
            symbol = signal.symbol
            strength = signal.strength
            target_price = signal.target_price
            stop_loss = signal.stop_loss
            
            # Record the trade
            self.trades.append({
                'date': date,
                'symbol': symbol,
                'type': 'BUY' if signal_type == strategies.SignalType.BUY else 
                        'SELL' if signal_type == strategies.SignalType.SELL else 'HOLD',
                'strength': strength,
                'target_price': target_price,
                'stop_loss': stop_loss
            })
        else:
            # Process Python signal
            # Assuming signal is a Python dictionary or object
            self.trades.append({
                'date': date,
                'symbol': signal.symbol,
                'type': signal.type,
                'strength': signal.strength,
                'target_price': signal.target_price,
                'stop_loss': signal.stop_loss
            })
    
    def _calculate_portfolio_value(self, portfolio, date, data):
        """Calculate the current portfolio value."""
        # This is a simplified implementation
        if self.use_cpp_bindings:
            # In a real implementation, this would get the value from the C++ portfolio object
            # For now, use a placeholder
            return self.initial_capital + len(self.trades) * 100  # Dummy calculation
        else:
            # Calculate portfolio value for Python portfolio
            return portfolio['cash'] + sum(portfolio['positions'].values())
    
    def _calculate_metrics(self):
        """Calculate performance metrics."""
        if not self.portfolio_value:
            return
            
        # Extract portfolio values as a series
        dates = [entry['date'] for entry in self.portfolio_value]
        values = [entry['value'] for entry in self.portfolio_value]
        portfolio_series = pd.Series(values, index=dates)
        
        # Calculate returns
        returns = portfolio_series.pct_change().dropna()
        
        # Basic metrics
        self.metrics = {
            'total_return': (portfolio_series.iloc[-1] / portfolio_series.iloc[0] - 1) * 100,
            'avg_daily_return': returns.mean() * 100,
            'volatility': returns.std() * 100,
            'sharpe_ratio': (returns.mean() / returns.std()) * np.sqrt(252) if returns.std() > 0 else 0,
            'max_drawdown': (portfolio_series / portfolio_series.cummax() - 1).min() * 100,
            'num_trades': len(self.trades)
        }
    
    def plot_performance(self, benchmark=None):
        """
        Plot portfolio performance over time.
        
        Args:
            benchmark: Optional benchmark data (pandas Series) to compare against
        """
        if not self.portfolio_value:
            raise ValueError("No backtest results to plot")
            
        # Create dataframe from portfolio history
        df = pd.DataFrame(self.portfolio_value)
        df['date'] = pd.to_datetime(df['date'])
        df.set_index('date', inplace=True)
        
        plt.figure(figsize=(12, 8))
        
        # Plot portfolio value
        plt.subplot(2, 1, 1)
        plt.plot(df.index, df['value'], label='Portfolio')
        
        # Plot benchmark if provided
        if benchmark is not None:
            if not isinstance(benchmark, pd.Series):
                raise ValueError("Benchmark must be a pandas Series with datetime index")
                
            # Reindex to match portfolio dates
            benchmark = benchmark.reindex(df.index, method='ffill')
            
            # Normalize to same starting value
            benchmark = benchmark / benchmark.iloc[0] * df['value'].iloc[0]
            plt.plot(df.index, benchmark, label='Benchmark')
            
        plt.title('Portfolio Value Over Time')
        plt.xlabel('Date')
        plt.ylabel('Value')
        plt.legend()
        plt.grid(True)
        
        # Plot drawdown
        plt.subplot(2, 1, 2)
        drawdown = df['value'] / df['value'].cummax() - 1
        plt.fill_between(df.index, 0, drawdown, color='red', alpha=0.3)
        plt.title('Drawdown')
        plt.xlabel('Date')
        plt.ylabel('Drawdown')
        plt.grid(True)
        
        plt.tight_layout()
        plt.show()
        
    def print_metrics(self):
        """Print performance metrics."""
        if not self.metrics:
            print("No metrics available. Run a backtest first.")
            return
            
        print("=== Performance Metrics ===")
        print(f"Total Return: {self.metrics['total_return']:.2f}%")
        print(f"Average Daily Return: {self.metrics['avg_daily_return']:.2f}%")
        print(f"Volatility (Daily): {self.metrics['volatility']:.2f}%")
        print(f"Sharpe Ratio (Annualized): {self.metrics['sharpe_ratio']:.2f}")
        print(f"Maximum Drawdown: {self.metrics['max_drawdown']:.2f}%")
        print(f"Number of Trades: {self.metrics['num_trades']}")
