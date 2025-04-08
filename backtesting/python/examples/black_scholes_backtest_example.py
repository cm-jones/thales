#!/usr/bin/env python3
"""
Example script demonstrating how to use the Thales backtesting framework
with the Black-Scholes Arbitrage strategy.

This example:
1. Loads historical options data
2. Creates a Black-Scholes Arbitrage strategy
3. Runs a backtest
4. Analyzes and visualizes the results
"""

import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import logging
from datetime import datetime, timedelta

# Add package to Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

# Import the backtesting framework
from src.thales_backtester.backtest.engine import BacktestEngine
from src.thales_backtester.models.black_scholes import BlackScholesModel


def main():
    # Configure logging
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    
    logger = logging.getLogger("BacktestExample")
    logger.info("Starting Black-Scholes backtest example")
    
    # Path to historical data
    data_path = os.path.abspath(os.path.join(
        os.path.dirname(__file__), 
        '..', '..', 'historical-data.csv'
    ))
    
    # Check if data file exists
    if not os.path.exists(data_path):
        logger.error(f"Historical data file not found at {data_path}")
        logger.info("Please download or generate sample options data and place it in the backtesting directory")
        return
    
    # Create the backtest engine
    engine = BacktestEngine(
        data_path=data_path,
        initial_capital=100000.0,
        commission=5.0  # $5 per trade
    )
    
    # Load historical data
    try:
        data = engine.load_data()
        logger.info(f"Loaded data with {len(data)} records")
        
        # Display sample of the data
        print("\nSample data:")
        print(data.head())
        
        # Show available symbols
        symbols = data['symbol'].unique()
        logger.info(f"Available symbols: {', '.join(symbols)}")
        
    except Exception as e:
        logger.error(f"Error loading data: {e}")
        return
    
    # Configure the strategy
    strategy_config = {
        'symbols': symbols.tolist(),
        'min_price_discrepancy': 0.10,  # 10% price difference
        'min_volatility': 0.15,
        'max_volatility': 0.60,
        'min_days_to_expiration': 7,
        'max_days_to_expiration': 45
    }
    
    # Define the backtest period
    # Use the full range of dates in the data as default
    start_date = data['date'].min()
    end_date = data['date'].max()
    
    # Run the backtest
    logger.info(f"Running backtest from {start_date} to {end_date}")
    results = engine.run_backtest(
        strategy="BlackScholesArbitrage",
        start_date=start_date,
        end_date=end_date,
        config=strategy_config
    )
    
    # Print performance metrics
    engine.print_metrics()
    
    # Analysis of trading signals
    trades = results['trades']
    if trades:
        trades_df = pd.DataFrame(trades)
        
        # Convert dates to datetime
        trades_df['date'] = pd.to_datetime(trades_df['date'])
        
        # Summary by signal type
        signal_summary = trades_df.groupby('type').size()
        print("\nSignal Summary:")
        print(signal_summary)
        
        # Summary by symbol
        if len(trades_df['symbol'].unique()) > 1:
            symbol_summary = trades_df.groupby('symbol').size().sort_values(ascending=False)
            print("\nTop Symbols:")
            print(symbol_summary.head(10))
        
        # Plot trade signals over time
        plt.figure(figsize=(12, 6))
        for signal_type in trades_df['type'].unique():
            subset = trades_df[trades_df['type'] == signal_type]
            plt.scatter(subset['date'], subset['strength'], label=signal_type, alpha=0.6)
            
        plt.title('Trading Signals Over Time')
        plt.xlabel('Date')
        plt.ylabel('Signal Strength')
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.savefig('signals_over_time.png')
        print("Saved trading signals plot to 'signals_over_time.png'")
    else:
        print("No trades were generated during the backtest period")
    
    # Plot performance
    try:
        # Create and save performance chart
        engine.plot_performance()
        plt.savefig('backtest_performance.png')
        print("Saved performance plot to 'backtest_performance.png'")
        
        # Show plots if running in interactive mode
        plt.show()
    except Exception as e:
        logger.error(f"Error plotting results: {e}")
    
    logger.info("Backtest example completed")


if __name__ == "__main__":
    main()
