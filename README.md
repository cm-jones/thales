# Thales

[![Build](https://github.com/cm-jones/thales/actions/workflows/00-build.yaml/badge.svg)](https://github.com/cm-jones/thales/actions/workflows/00-build.yaml) [![codecov](https://codecov.io/gh/cm-jones/thales/branch/main/graph/badge.svg)](https://codecov.io/gh/cm-jones/thales) [![CodeFactor](https://www.codefactor.io/repository/github/cm-jones/thales/badge)](https://www.codefactor.io/repository/github/cm-jones/thales) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Thales is an ultra low-latency, multi-threaded, and extensible automated trading bot designed solely for trading options contracts (not equities or any other financial instruments) via the Interactive Brokers API.

<small>_Disclaimer: Trading options involves significant risk of loss and is not suitable for all investors. Past performance is not indicative of future results. This software is for educational and informational purposes only and should not be considered investment advice._</small>

## Features

- **IB Integration**: Connect to Interactive Brokers API for real-time market data and order execution
- **Black-Scholes Model**: Accurate options pricing and Greeks calculation
- **Multiple Trading Strategies**: Implement and test various options trading strategies
- **Risk Management**: Sophisticated risk controls to protect your capital
- **Backtesting**: Test strategies against historical data
- **PostgreSQL Database Logging**: Log trade executions to a PostgreSQL database for analysis and reporting
- **Extensible Architecture**: Easily add new strategies and components

## Development Environment

### Using Dev Container (Recommended)

The project includes a dev container configuration that provides a standardized development environment with all required dependencies pre-installed. This is the recommended way to develop for Thales.

#### Prerequisites

- [Docker](https://www.docker.com/get-started)
- [Visual Studio Code](https://code.visualstudio.com/)
- [VS Code Remote - Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

#### Getting Started with Dev Container

1. Clone the repository:

   ```bash
   git clone https://github.com/cm-jones/thales.git
   cd thales
   ```

2. Open the project in VS Code:

   ```bash
   code .
   ```

3. When prompted "Reopen in Container", click "Reopen in Container". Alternatively:

   - Press F1
   - Type "Reopen in Container"
   - Select "Remote-Containers: Reopen in Container"

4. Wait for the container to build. This will:
   - Set up a complete development environment
   - Install all required dependencies
   - Configure build tools and extensions
   - Set up debugging support

The dev container provides:

- All required build tools and libraries
- Consistent compiler version and flags
- Pre-configured development environment
- Debug configurations
- Code formatting and linting tools

## Requirements

- An Interactive Brokers account and TWS or IB Gateway
- C++23 compatible compiler
- CMake 3.12 or higher
- Boost libraries
- nlohmann/json (for configuration parsing)
- (Optional) PostgreSQL 10+ (for database logging)
- (Optional) libpqxx (for PostgreSQL connectivity)

All of these requirements are automatically handled if you use the recommended dev container setup above.

### Manual Setup

If you prefer not to use the dev container, you'll need to install all the requirements listed above manually. Instructions for installing each dependency can be found in their respective documentation.

## Build

> **Note:** The following build instructions are provided for reference. We strongly recommend using the dev container setup described above for the best development experience, as it provides a pre-configured environment with all dependencies and proper tooling.

### Manual Build on Linux

```bash
# Clone the repository
git clone https://github.com/cm-jones/thales.git
cd thales

# Install the Interactive Brokers C++ API
./scripts/install/install_ibapi.sh

# Create a build directory
mkdir -p build && cd build

# Configure and build
cmake ..
make

# Run the trading bot
./thales
```

### Running Tests

```bash
# Run all tests
./scripts/test/test.sh

# Run tests with coverage reporting
./scripts/test/coverage.sh
```

The coverage report will be generated in `build/coverage_report/index.html`. Open this file in a browser to view the detailed coverage report.

## Setting up Interactive Brokers

Thales integrates with Interactive Brokers for market data and trade execution. To use this feature:

1. **Install TWS or IB Gateway**:

   - Download and install [Interactive Brokers TWS](https://www.interactivebrokers.com/en/trading/tws.php) or [IB Gateway](https://www.interactivebrokers.com/en/trading/ibgateway.php)
   - TWS is recommended for manual trading alongside Thales
   - IB Gateway is more lightweight and suitable for automated trading only

2. **Configure API Settings**:

   - In TWS/IB Gateway, go to Edit > Global Configuration > API > Settings
   - Check "Enable ActiveX and Socket Clients"
   - Set "Socket port" to match the port in your config.json (default: 7496)
   - Check "Allow connections from localhost only" for security
   - Uncheck "Read-Only API" to allow trading

3. (Optional) **Paper Trading**:

   - Create a paper trading account in TWS/IB Gateway for testing
   - Go to File > Login > Paper Trading
   - This allows you to test Thales without risking real money

4. **API Permissions**:

   - In TWS/IB Gateway, go to Edit > Global Configuration > API > Precautions
   - Configure which operations you want to allow

5. **Update config.json**:
   - Set `data.ibHost` to the IP address of your TWS/IB Gateway (usually "127.0.0.1")
   - Set `data.ibPort` to the socket port you configured (default: 7496)
   - Set `data.ibClientId` to a unique ID (important if you have multiple applications connecting)
   - Set `data.ibAccount` to your IB account number

## Configuration

Thales is configured through a JSON file located in the `config` directory. The default configuration file is `config/config.json`. You can specify a different configuration file by passing it as a command-line argument:

```bash
./thales path/to/your/config.json
```

See [config/README.md](config/README.md) for details.

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
#pragma once

#include "thales/strategies/strategy_base.hpp"

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
```

## Performance Benchmarks

Thales includes performance benchmarks for critical components using Google Benchmark. These benchmarks help measure and optimize the performance of key algorithms, particularly the Black-Scholes model implementation.

### Running Benchmarks

```bash
# Run the benchmarks
./scripts/performance/run_benchmarks.sh
```

This will build the benchmarks in Release mode and execute them, providing detailed performance metrics for various operations.

The benchmarks include:

- Option pricing (call and put)
- Greeks calculation (delta, gamma, theta, vega, rho)
- Implied volatility calculation
- Performance across different moneyness levels, volatility values, and expiration times

### Interpreting Results

The benchmark results show:

- Time per iteration (lower is better)
- CPU time consumed
- Number of iterations performed
- Comparisons between different parameter sets

Use these benchmarks to identify performance bottlenecks and validate optimizations.

## Trade Logging

Thales can log trade executions to a PostgreSQL database for analysis and reporting. The database will be created automatically when the bot starts if it doesn't exist.

### Quick Setup

1. Install PostgreSQL and libpqxx:

   ```bash
   # Install PostgreSQL
   ./scripts/database/setup_postgres.sh

   # Install libpqxx
   ./scripts/install/install_libpqxx.sh
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
./scripts/database/setup_postgres.sh
```

The database includes the following features:

- Trade execution logging with detailed information
- Indexes for efficient querying
- Views for common queries (e.g., recent trades)
- Functions for analysis (e.g., calculating daily P&L)

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
