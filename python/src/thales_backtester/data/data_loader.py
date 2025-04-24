import pandas as pd
import numpy as np
from datetime import datetime, timedelta
import os
from typing import Dict, List, Optional, Union, Tuple


class OptionsDataLoader:
    """
    Class for loading and preprocessing historical options data.
    """
    
    def __init__(self, data_dir: str = None):
        """
        Initialize the data loader.
        
        Args:
            data_dir: Directory containing historical options data files
        """
        self.data_dir = data_dir or os.path.join(os.path.dirname(os.path.dirname(
                                   os.path.dirname(os.path.abspath(__file__)))), 
                                   'data')
        self.data = None
        self.symbols = set()
        
    def load_csv(self, filepath: str = None, **kwargs) -> pd.DataFrame:
        """
        Load options data from CSV file.
        
        Args:
            filepath: Path to CSV file. If None, uses data_dir
            **kwargs: Additional arguments to pass to pd.read_csv
            
        Returns:
            DataFrame containing options data
        """
        filepath = filepath or self.data_dir
        
        # Default parameters for reading CSV files
        default_params = {
            'parse_dates': ['date', 'expiration'],
            'infer_datetime_format': True
        }
        # Override defaults with any provided kwargs
        params = {**default_params, **kwargs}
        
        try:
            data = pd.read_csv(filepath, **params)
            
            # Basic validation of expected columns
            required_cols = ['date', 'symbol', 'expiration', 'strike', 'option_type', 
                            'bid', 'ask', 'volume', 'open_interest', 'underlying_price']
            
            missing_cols = [col for col in required_cols if col not in data.columns]
            if missing_cols:
                print(f"Warning: Missing expected columns: {missing_cols}")
                print(f"Available columns: {data.columns.tolist()}")
            
            # Add calculated columns
            if 'bid' in data.columns and 'ask' in data.columns:
                data['mid'] = (data['bid'] + data['ask']) / 2
                
            if 'symbol' in data.columns:
                self.symbols = set(data['symbol'].unique())
                
            self.data = data
            return data
            
        except Exception as e:
            print(f"Error loading data from {filepath}: {e}")
            # Return empty DataFrame with expected columns
            return pd.DataFrame(columns=required_cols)
    
    def filter_data(self,
                   symbols: List[str] = None,
                   start_date: Union[str, datetime] = None,
                   end_date: Union[str, datetime] = None,
                   min_days_to_expiry: int = None,
                   max_days_to_expiry: int = None,
                   option_types: List[str] = None) -> pd.DataFrame:
        """
        Filter the loaded options data based on various criteria.
        
        Args:
            symbols: List of symbols to include
            start_date: Start date for filtering
            end_date: End date for filtering
            min_days_to_expiry: Minimum days to expiration
            max_days_to_expiry: Maximum days to expiration
            option_types: List of option types to include ('call', 'put')
            
        Returns:
            Filtered DataFrame
        """
        if self.data is None:
            print("No data loaded. Call load_csv() first.")
            return pd.DataFrame()
            
        data = self.data.copy()
        
        # Apply filters
        if symbols:
            data = data[data['symbol'].isin(symbols)]
            
        if start_date:
            if isinstance(start_date, str):
                start_date = pd.to_datetime(start_date)
            data = data[data['date'] >= start_date]
            
        if end_date:
            if isinstance(end_date, str):
                end_date = pd.to_datetime(end_date)
            data = data[data['date'] <= end_date]
        
        if 'date' in data.columns and 'expiration' in data.columns:
            data['days_to_expiry'] = (data['expiration'] - data['date']).dt.days
            
            if min_days_to_expiry is not None:
                data = data[data['days_to_expiry'] >= min_days_to_expiry]
                
            if max_days_to_expiry is not None:
                data = data[data['days_to_expiry'] <= max_days_to_expiry]
        
        if option_types:
            option_types = [opt.lower() for opt in option_types]
            data = data[data['option_type'].str.lower().isin(option_types)]
            
        return data
    
    def get_option_chain(self, 
                        symbol: str,
                        date: Union[str, datetime],
                        expiration: Union[str, datetime] = None) -> pd.DataFrame:
        """
        Get the option chain for a specific symbol and date.
        
        Args:
            symbol: Underlying symbol
            date: Date for which to get the option chain
            expiration: Specific expiration date to filter (optional)
            
        Returns:
            DataFrame with the option chain
        """
        if self.data is None:
            print("No data loaded. Call load_csv() first.")
            return pd.DataFrame()
            
        if isinstance(date, str):
            date = pd.to_datetime(date)
            
        # Get data for the specific date and symbol
        mask = (self.data['date'] == date) & (self.data['symbol'] == symbol)
        
        if expiration:
            if isinstance(expiration, str):
                expiration = pd.to_datetime(expiration)
            mask &= (self.data['expiration'] == expiration)
            
        chain = self.data[mask].copy()
        
        # Sort by strike and option type
        if not chain.empty and 'strike' in chain.columns:
            chain = chain.sort_values(['strike', 'option_type'])
            
        return chain
    
    def get_underlying_prices(self, 
                             symbol: str,
                             start_date: Union[str, datetime],
                             end_date: Union[str, datetime]) -> pd.Series:
        """
        Get the underlying prices for a specific symbol and date range.
        
        Args:
            symbol: Underlying symbol
            start_date: Start date
            end_date: End date
            
        Returns:
            Series with underlying prices indexed by date
        """
        if self.data is None:
            print("No data loaded. Call load_csv() first.")
            return pd.Series()
            
        if isinstance(start_date, str):
            start_date = pd.to_datetime(start_date)
        
        if isinstance(end_date, str):
            end_date = pd.to_datetime(end_date)
            
        # Filter by date range and symbol
        mask = (
            (self.data['date'] >= start_date) & 
            (self.data['date'] <= end_date) & 
            (self.data['symbol'] == symbol)
        )
        
        filtered = self.data[mask]
        
        if filtered.empty:
            return pd.Series()
            
        # Get unique date and price pairs
        prices = filtered[['date', 'underlying_price']].drop_duplicates('date')
        
        # Return as a Series indexed by date
        return pd.Series(prices['underlying_price'].values, index=prices['date'])
