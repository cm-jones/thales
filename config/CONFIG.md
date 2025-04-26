# Configuration Options

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
