#!/bin/bash
# Script to set up PostgreSQL for Thales

set -e

echo "Setting up PostgreSQL for Thales..."

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    echo "Detected Linux system"
    
    # Check if PostgreSQL is installed
    if ! command -v psql &> /dev/null; then
        echo "PostgreSQL not found. Installing..."
        sudo apt-get update
        sudo apt-get install -y postgresql postgresql-contrib
    else
        echo "PostgreSQL is already installed"
    fi
    
    # Start PostgreSQL service
    sudo systemctl start postgresql
    sudo systemctl enable postgresql
    
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    echo "Detected macOS system"
    
    # Check if Homebrew is installed
    if ! command -v brew &> /dev/null; then
        echo "Homebrew not found. Installing..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    else
        echo "Homebrew is already installed"
    fi
    
    # Check if PostgreSQL is installed
    if ! command -v psql &> /dev/null; then
        echo "PostgreSQL not found. Installing..."
        brew install postgresql
    else
        echo "PostgreSQL is already installed"
    fi
    
    # Start PostgreSQL service
    brew services start postgresql
    
elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    # Windows
    echo "Detected Windows system"
    echo "Please install PostgreSQL manually from https://www.postgresql.org/download/windows/"
    echo "After installation, use the following commands to set up the database:"
    echo "1. Create a user: CREATE USER thales_user WITH PASSWORD 'your_secure_password';"
    echo "2. Create a database: CREATE DATABASE thales OWNER thales_user;"
    echo "3. Connect to the database and run the SQL commands below"
    exit 0
else
    echo "Unsupported operating system: $OSTYPE"
    exit 1
fi

echo "Creating database and user..."

# Define variables
DB_NAME="thales"
DB_USER="thales_user"
DB_PASSWORD="thales_password" # Change this in production

# Create the database and user with sudo (Linux) or as current user (macOS)
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # For Linux, use sudo to run as postgres user
    sudo -u postgres psql -c "CREATE USER $DB_USER WITH PASSWORD '$DB_PASSWORD';" || echo "User already exists"
    sudo -u postgres psql -c "CREATE DATABASE $DB_NAME OWNER $DB_USER;" || echo "Database already exists"
else
    # For macOS, run as current user
    psql postgres -c "CREATE USER $DB_USER WITH PASSWORD '$DB_PASSWORD';" || echo "User already exists"
    psql postgres -c "CREATE DATABASE $DB_NAME OWNER $DB_USER;" || echo "Database already exists"
fi

echo "Creating tables..."

# Create tables SQL
SQL_CREATE_TABLES="
-- Trades table
CREATE TABLE IF NOT EXISTS trades (
    id SERIAL PRIMARY KEY,
    symbol VARCHAR(20) NOT NULL,
    contract_id INTEGER NOT NULL,
    strategy VARCHAR(50) NOT NULL,
    order_id VARCHAR(50) NOT NULL,
    direction VARCHAR(10) NOT NULL,
    quantity INTEGER NOT NULL,
    price DECIMAL(10, 2) NOT NULL,
    commission DECIMAL(10, 2) NOT NULL,
    execution_time TIMESTAMP NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create index on symbol for faster querying
CREATE INDEX IF NOT EXISTS idx_trades_symbol ON trades(symbol);

-- Create index on execution_time for time-based queries
CREATE INDEX IF NOT EXISTS idx_trades_execution_time ON trades(execution_time);

-- Positions table
CREATE TABLE IF NOT EXISTS positions (
    id SERIAL PRIMARY KEY,
    symbol VARCHAR(20) NOT NULL,
    contract_id INTEGER NOT NULL,
    quantity INTEGER NOT NULL,
    average_price DECIMAL(10, 2) NOT NULL,
    current_price DECIMAL(10, 2) NOT NULL,
    unrealized_pnl DECIMAL(10, 2) NOT NULL,
    realized_pnl DECIMAL(10, 2) NOT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(symbol, contract_id)
);

-- Create index on symbol for faster querying
CREATE INDEX IF NOT EXISTS idx_positions_symbol ON positions(symbol);

-- Portfolio value history
CREATE TABLE IF NOT EXISTS portfolio_history (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    cash_balance DECIMAL(15, 2) NOT NULL,
    positions_value DECIMAL(15, 2) NOT NULL,
    total_value DECIMAL(15, 2) NOT NULL
);

-- Create index on timestamp for time-based queries
CREATE INDEX IF NOT EXISTS idx_portfolio_history_timestamp ON portfolio_history(timestamp);

-- Add a view for daily P&L
CREATE OR REPLACE VIEW daily_pnl AS
SELECT 
    DATE(execution_time) AS trade_date,
    SUM((CASE WHEN direction = 'BUY' THEN -1 ELSE 1 END) * quantity * price - commission) AS daily_pnl
FROM 
    trades
GROUP BY 
    DATE(execution_time)
ORDER BY 
    trade_date DESC;

-- Add a function to calculate P&L for a date range
CREATE OR REPLACE FUNCTION calculate_pnl(start_date DATE, end_date DATE)
RETURNS DECIMAL(15, 2) AS $$
DECLARE
    total_pnl DECIMAL(15, 2);
BEGIN
    SELECT 
        SUM((CASE WHEN direction = 'BUY' THEN -1 ELSE 1 END) * quantity * price - commission)
    INTO 
        total_pnl
    FROM 
        trades
    WHERE 
        DATE(execution_time) BETWEEN start_date AND end_date;
    
    RETURN COALESCE(total_pnl, 0);
END;
$$ LANGUAGE plpgsql;
"

# Run SQL for table creation
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # For Linux, use sudo to run as postgres user
    echo "$SQL_CREATE_TABLES" | sudo -u postgres psql -d "$DB_NAME"
else
    # For macOS, run as current user
    echo "$SQL_CREATE_TABLES" | psql -d "$DB_NAME"
fi

echo "PostgreSQL setup completed successfully!"
echo "Database: $DB_NAME"
echo "User: $DB_USER"
echo "Password: $DB_PASSWORD (Change this in production)"
echo ""
echo "Update your config.json with these database settings:"
echo '{
  "logging": {
    "database": {
      "enabled": true,
      "host": "localhost",
      "port": 5432,
      "name": "'$DB_NAME'",
      "user": "'$DB_USER'",
      "password": "'$DB_PASSWORD'"
    }
  }
}'
