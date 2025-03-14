# Thales

[![Build](https://github.com/cm-jones/thales/actions/workflows/build.yml/badge.svg)](https://github.com/cm-jones/thales/actions/workflows/build.yml) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Thales is a high-frequency, algorithmic trading bot designed for options contracts, named after the Greek philosopher [Thales of Miletus](https://en.wikipedia.org/wiki/Thales_of_Miletus), who is credited with the first recorded use of an options contract.

## Features

- **Interactive Brokers Integration**: Connect to Interactive Brokers API for real-time market data and order execution
- **Black-Scholes Model**: Accurate options pricing and Greeks calculation
- **Multiple Trading Strategies**: Implement and test various options trading strategies
- **Risk Management**: Sophisticated risk controls to protect your capital
- **Backtesting**: Test strategies against historical data
- **PostgreSQL Database Logging**: Log trade executions to a PostgreSQL database for analysis and reporting
- **Extensible Architecture**: Easily add new strategies and components

## Requirements

- C++17 compatible compiler (GCC 8+, Clang 7+, MSVC 2019+)
- CMake 3.12 or higher
- Interactive Brokers TWS or IB Gateway
- Boost libraries (for networking and datetime handling)
- nlohmann/json (for configuration parsing)
- PostgreSQL 10+ (optional, for database logging)
- libpqxx (optional, for PostgreSQL connectivity)

## Build

### Unix

```bash
# Clone the repository
git clone https://github.com/cm-jones/thales.git
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
git clone https://github.com/cm-jones/thales.git
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
  - `loop_interval_ms`: Main loop interval in milliseconds
  - `max_threads`: Maximum number of threads to use

- **Risk**: Risk management settings
  - `max_position_size`: Maximum position size in dollars
  - `max_drawdown`: Maximum allowed drawdown as a fraction of portfolio value
  - `max_leverage`: Maximum allowed leverage
  - `max_risk_per_trade`: Maximum risk per trade as a fraction of portfolio value
  - `max_daily_loss`: Maximum allowed daily loss as a fraction of portfolio value

- **Data**: Market data settings
  - `ib_host`: Interactive Brokers TWS/Gateway host
  - `ib_port`: Interactive Brokers TWS/Gateway port
  - `ib_client_id`: Interactive Brokers client ID
  - `ib_account`: Interactive Brokers account ID
  - `symbols`: List of symbols to trade
  - `bar_sizes`: List of bar sizes to request
  - `history_days`: Number of days of historical data to request

- **Strategies**: Trading strategy settings
  - `enabled`: List of enabled strategies
  - Strategy-specific settings

- **Logging**: Logging settings
  - `log_to_file`: Whether to log to a file
  - `log_file_path`: Path to the log file
  - `console_log_level`: Minimum log level for console output
  - `file_log_level`: Minimum log level for file output
  - `database`: Database logging settings
    - `enabled`: Whether to enable database logging
    - `host`: Database host
    - `port`: Database port
    - `name`: Database name
    - `user`: Database user
    - `password`: Database password
    - `max_queue_size`: Maximum size of the log queue
    - `batch_size`: Number of logs to insert in a batch

- **Backtesting**: Backtesting settings
  - `enabled`: Whether to run in backtesting mode
  - `start_date`: Start date for backtesting
  - `end_date`: End date for backtesting
  - `initial_capital`: Initial capital for backtesting
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
  - **setup_postgres.sh**: Script to set up PostgreSQL database
  - **setup_database.sql**: SQL script to create database tables and functions
  - **install_libpqxx.sh**: Script to install libpqxx library
  - **build_with_asan.sh**: Script to build with Address Sanitizer
  - **run_with_asan.sh**: Script to run with Address Sanitizer
  - **run_tests_with_asan.sh**: Script to run tests with Address Sanitizer
- **third_party/**: Third-party dependencies
- **examples/**: Example usage and sample strategies
- **docs/**: Documentation
  - **address_sanitizer.md**: Documentation for using Address Sanitizer

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

## Database Logging

Thales can log trade executions to a PostgreSQL database for analysis and reporting. The database will be created automatically when the bot starts if it doesn't exist.

### Quick Setup

1. Install PostgreSQL and libpqxx:
   ```bash
   # Install PostgreSQL
   ./scripts/setup_postgres.sh
   
   # Install libpqxx
   ./scripts/install_libpqxx.sh
   ```

2. Configure database logging in `config/config.json`:
   ```json
   "logging": {
     "database": {
       "enabled": true,
       "host": "localhost",
       "port": 5432,
       "name": "thales",
       "user": "thales_user",
       "password": "your_secure_password"
     }
   }
   ```

3. Rebuild the project:
   ```bash
   cd build
   cmake ..
   make
   ```

4. Run the trading bot:
   ```bash
   ./thales
   ```

### Automatic Database Creation

The bot will automatically:
- Create the database if it doesn't exist
- Create the necessary tables and indexes
- Set up views and functions for analysis

This requires:
- PostgreSQL to be installed and running
- The configured database user to have permission to create databases
- The libpqxx library to be installed

If automatic creation fails, you can run the setup script manually:
```bash
./scripts/setup_postgres.sh
```

The database includes the following features:
- Trade execution logging with detailed information
- Indexes for efficient querying
- Views for common queries (e.g., recent trades)
- Functions for analysis (e.g., calculating daily P&L)

## Memory Safety with Address Sanitizer

Thales includes support for Address Sanitizer (ASan), a memory error detector for C/C++ that can find various memory bugs like use-after-free, heap buffer overflow, stack buffer overflow, memory leaks, etc.

### Building with Address Sanitizer

```bash
# Build with Address Sanitizer
./scripts/build_with_asan.sh

# Run with Address Sanitizer
./scripts/run_with_asan.sh

# Run tests with Address Sanitizer
./scripts/run_tests_with_asan.sh
```

For more details, see [docs/address_sanitizer.md](docs/address_sanitizer.md).

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Disclaimer

Trading options involves significant risk of loss and is not suitable for all investors. Past performance is not indicative of future results. This software is for educational and informational purposes only and should not be considered investment advice.
