#!/bin/bash
set -e

# Set default values if not provided
DB_HOST=${DB_HOST:-localhost}
DB_USER=${DB_USER:-thales}
DB_PASSWORD=${DB_PASSWORD:-thales}
DB_NAME=${DB_NAME:-thales}

# Wait for PostgreSQL to be ready
echo "Waiting for PostgreSQL at ${DB_HOST}..."
until PGPASSWORD=$DB_PASSWORD psql -h $DB_HOST -U $DB_USER -d $DB_NAME -c '\q' 2>/dev/null; do
  echo "PostgreSQL is unavailable - sleeping"
  sleep 1
done

echo "PostgreSQL is up - starting Thales"

# Start the trading bot with optimized settings
exec /usr/local/bin/thales /etc/thales/config/config.json
