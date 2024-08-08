# Thales: an Algorithmic Options Trading Bot

![build](https://github.com/cm-jones/thales/actions/workflows/build.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Description

Thales (*THAY-leez*) is a high-frequency, algorithmic trading bot designed for trading options contracts, named after the Greek philosopher [Thales of Miletus](https://en.wikipedia.org/wiki/Thales_of_Miletus), who is credited with the first recorded use of an options contract.

The bot uses the Black-Scholes formula and various statistical models to identify options that are "mispriced" by the market. For example, if the bot thinks a call or put is currently undervalued, it will attempt to buy that call or put.

## Requirements

## Usage

## Configuration

Thales can be configured through various configuration files inside the `config` directory. Here's a brief overview of the available configuration options:

- `polygon_credentials.cfg` (required): Contains the Polygon.io API key for accessing real-time and historical options data.
- `trading_strategy.cfg`: Defines the parameters and settings for the trading strategy.
- `risk_management.cfg`: Specifies risk management rules and thresholds.
- `data_sources.cfg`: Configures the data sources for historical and real-time market data.

Refer to the individual configuration files and the project documentation for more details on configuring the trading bot.

## Build

1. Clone the repository:
   ```sh
   git clone https://github.com/cm-jones/thales.git
   cd thales
   ```

## Testing

## Benchmarking

## Documentation

## Contributing

See [CONTRIBUTING.md](https://github.com/cm-jones/thales/CONTRIBUTING.md) for information on how to contribute to Thales.

## License

This project is licensed under the [MIT License](LICENSE).
