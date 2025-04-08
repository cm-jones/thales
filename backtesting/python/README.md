# Thales Python Backtesting Framework

This package provides a Python backtesting framework for options trading strategies using the Thales trading system. It integrates with the existing C++ models and strategies through pybind11 bindings, allowing for high-performance backtesting while providing the flexibility and ease of use of Python.

## Features

- Integration with C++ models and strategies through pybind11 bindings
- Pure Python implementations as fallbacks
- Historical options data loading and processing
- Comprehensive backtesting engine
- Performance metrics and visualization
- Support for custom strategies

## Structure

The package is organized as follows:

```
thales_backtester/
├── data/               # Data loading and processing
├── models/             # Option pricing models
├── strategies/         # Trading strategies
├── backtest/           # Backtesting engine
├── utils/              # Utility functions
└── bindings/           # C++ bindings
```

## Installation

### Prerequisites

- Python 3.8 or later
- C++ compiler with C++17 support
- CMake 3.14 or later
- pybind11

### Building from Source

Clone the Thales repository and navigate to the backtesting/python directory:

```bash
cd /path/to/thales/backtesting/python
pip install -e .
```

This will build the C++ bindings and install the package in development mode.

## Usage

### Basic Example

```python
from thales_backtester.backtest.engine import BacktestEngine

# Create the backtest engine
engine = BacktestEngine(
    data_path="path/to/historical-data.csv",
    initial_capital=100000.0,
    commission=5.0
)

# Load historical data
data = engine.load_data()

# Configure the strategy
strategy_config = {
    'symbols': ['SPY', 'AAPL'],
    'min_price_discrepancy': 0.10,
    'min_volatility': 0.15,
    'max_volatility': 0.60,
    'min_days_to_expiration': 7,
    'max_days_to_expiration': 45
}

# Run the backtest
results = engine.run_backtest(
    strategy="BlackScholesArbitrage",
    start_date="2023-01-01",
    end_date="2023-12-31",
    config=strategy_config
)

# Print performance metrics
engine.print_metrics()

# Plot performance
engine.plot_performance()
```

See the `examples` directory for more detailed examples.

### Data Format

The backtesting engine expects historical options data in CSV format with the following columns:

- `date`: The date of the observation
- `symbol`: The underlying symbol
- `expiration`: The option expiration date
- `strike`: The option strike price
- `option_type`: The option type ('call' or 'put')
- `bid`: The bid price
- `ask`: The ask price
- `volume`: The trading volume
- `open_interest`: The open interest
- `underlying_price`: The price of the underlying asset

Additional columns may be included and will be preserved in the loaded data.

## Implementing Custom Strategies

To implement a custom strategy, you can either:

1. Create a new C++ strategy in the Thales codebase and expose it through the bindings
2. Create a Python strategy class that inherits from `StrategyBase` in the `strategies` module

### Python Strategy Example

```python
from thales_backtester.strategies import StrategyBase, Signal

class MyCustomStrategy(StrategyBase):
    def __init__(self, config):
        super().__init__("MyCustomStrategy", config)
        # Initialize strategy parameters
        
    def initialize(self):
        # Perform any necessary initialization
        return True
        
    def execute(self, market_data, portfolio):
        # Implement your strategy logic
        signals = []
        # Generate signals based on market data
        return signals
```

## License

This software is licensed under the same license as the Thales trading system.
