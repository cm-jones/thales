# Thales: Options Trading Bot

Thales is a high-frequency, algorithmic trading bot designed for trading options contracts, named after the Greek philosopher [Thales of Miletus](https://en.wikipedia.org/wiki/Thales_of_Miletus), who is credited with the first recorded use of an options contract.

## Features

- **Interactive Brokers Integration**: Connect to Interactive Brokers API for real-time market data and order execution
- **Black-Scholes Model**: Accurate options pricing and Greeks calculation
- **Multiple Trading Strategies**: Implement and test various options trading strategies
- **Risk Management**: Sophisticated risk controls to protect your capital
- **Backtesting**: Test strategies against historical data
- **Extensible Architecture**: Easily add new strategies and components

## Requirements

- C++17 compatible compiler (GCC 8+, Clang 7+, MSVC 2019+)
- CMake 3.12 or higher
- Interactive Brokers TWS or IB Gateway
- Boost libraries (for networking and datetime handling)
- nlohmann/json (for configuration parsing)

## Building from Source

### Linux/macOS

```bash
# Clone the repository
git clone https://github.com/yourusername/thales.git
cd thales

# Create a build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the trading bot
./thales
```

### Windows

```bash
# Clone the repository
git clone https://github.com/yourusername/thales.git
cd thales

# Create a build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build . --config Release

# Run the trading bot
Release\thales.exe
```

## Configuration

Thales is configured through a JSON file located in the `config` directory. The default configuration file is `config/config.json`. You can specify a different configuration file by passing it as a command-line argument:

```bash
./thales path/to/your/config.json
```

### Configuration Options

- **Engine**: General settings for the trading engine
  - `loopIntervalMs`: Main loop interval in milliseconds
  - `maxThreads`: Maximum number of threads to use

- **Risk**: Risk management settings
  - `maxPositionSize`: Maximum position size in dollars
  - `maxDrawdown`: Maximum allowed drawdown as a fraction of portfolio value
  - `maxLeverage`: Maximum allowed leverage
  - `maxRiskPerTrade`: Maximum risk per trade as a fraction of portfolio value
  - `maxDailyLoss`: Maximum allowed daily loss as a fraction of portfolio value

- **Data**: Market data settings
  - `ibHost`: Interactive Brokers TWS/Gateway host
  - `ibPort`: Interactive Brokers TWS/Gateway port
  - `ibClientId`: Interactive Brokers client ID
  - `ibAccount`: Interactive Brokers account ID
  - `symbols`: List of symbols to trade
  - `barSizes`: List of bar sizes to request
  - `historyDays`: Number of days of historical data to request

- **Strategies**: Trading strategy settings
  - `enabled`: List of enabled strategies
  - Strategy-specific settings

- **Logging**: Logging settings
  - `logToFile`: Whether to log to a file
  - `logFilePath`: Path to the log file
  - `consoleLogLevel`: Minimum log level for console output
  - `fileLogLevel`: Minimum log level for file output

- **Backtesting**: Backtesting settings
  - `enabled`: Whether to run in backtesting mode
  - `startDate`: Start date for backtesting
  - `endDate`: End date for backtesting
  - `initialCapital`: Initial capital for backtesting
  - `commission`: Commission rate for backtesting
  - `slippage`: Slippage rate for backtesting

## Project Structure

- **src/**: Source files
  - **core/**: Core functionality (engine, execution, risk management)
  - **models/**: Options pricing models (Black-Scholes, etc.)
  - **strategies/**: Trading strategy implementations
  - **data/**: Market data handling and IB API integration
  - **backtesting/**: Backtesting framework
  - **utils/**: Utility functions and helpers

- **include/thales/**: Header files
  - Mirrors the src/ structure for clean organization

- **tests/**: Unit and integration tests
- **config/**: Configuration files
- **scripts/**: Build and utility scripts
- **third_party/**: Third-party dependencies
- **examples/**: Example usage and sample strategies
- **docs/**: Documentation

## Adding a New Strategy

To add a new trading strategy:

1. Create a new header file in `include/thales/strategies/`
2. Create a new source file in `src/strategies/`
3. Inherit from the `StrategyBase` class
4. Implement the required methods
5. Register the strategy in the `StrategyRegistry`
6. Add the strategy to the enabled strategies list in the configuration file

Example:

```cpp
// include/thales/strategies/my_strategy.h
#ifndef THALES_STRATEGIES_MY_STRATEGY_H
#define THALES_STRATEGIES_MY_STRATEGY_H

#include <thales/strategies/strategy_base.h>

namespace thales {
namespace strategies {

class MyStrategy : public StrategyBase {
public:
    MyStrategy(const utils::Config& config);
    ~MyStrategy() override = default;
    
    bool initialize() override;
    std::vector<Signal> execute(
        const std::vector<data::MarketData>& marketData,
        const core::Portfolio& portfolio
    ) override;
    std::vector<std::string> getSymbols() const override;
    
private:
    // Strategy-specific members
};

} // namespace strategies
} // namespace thales

#endif // THALES_STRATEGIES_MY_STRATEGY_H
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Disclaimer

Trading options involves significant risk of loss and is not suitable for all investors. Past performance is not indicative of future results. This software is for educational and informational purposes only and should not be considered investment advice.
