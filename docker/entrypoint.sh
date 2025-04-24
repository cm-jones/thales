#!/bin/bash
set -e

# Set default values if not provided
DB_HOST=${DB_HOST:-localhost}
DB_USER=${DB_USER:-thales}
DB_PASSWORD=${DB_PASSWORD:-thales}
DB_NAME=${DB_NAME:-thales}
MAX_RETRIES=30
RETRY_INTERVAL=2

# Function to log messages with timestamp
log() {
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] $1"
}

# Function to handle cleanup on exit
cleanup() {
    log "Shutting down Thales..."
    if [ -f /var/run/thales.pid ]; then
        kill -TERM $(cat /var/run/thales.pid) 2>/dev/null || true
    fi
}

# Set up signal handling
trap cleanup SIGTERM SIGINT SIGQUIT

# Function to check PostgreSQL connection
check_postgres() {
    PGPASSWORD=$DB_PASSWORD psql -h $DB_HOST -U $DB_USER -d $DB_NAME -c '\q' 2>/dev/null
}

# Wait for PostgreSQL with timeout
log "Waiting for PostgreSQL at ${DB_HOST}..."
RETRIES=0
until check_postgres || [ $RETRIES -eq $MAX_RETRIES ]; do
    RETRIES=$((RETRIES+1))
    log "PostgreSQL is unavailable - retry $RETRIES/$MAX_RETRIES"
    sleep $RETRY_INTERVAL
done

if [ $RETRIES -eq $MAX_RETRIES ]; then
    log "ERROR: Failed to connect to PostgreSQL after $MAX_RETRIES attempts"
    exit 1
fi

log "PostgreSQL is up - performing health check..."

# Additional health check queries
if ! PGPASSWORD=$DB_PASSWORD psql -h $DB_HOST -U $DB_USER -d $DB_NAME -c 'SELECT 1' >/dev/null 2>&1; then
    log "ERROR: Database health check failed"
    exit 1
fi

log "Database connection successful"

# Verify directories exist and are writable
for dir in "/usr/local/thales/logs" "/usr/local/thales/data"; do
    if [ ! -d "$dir" ]; then
        log "ERROR: Directory $dir does not exist"
        exit 1
    fi
    if [ ! -w "$dir" ]; then
        log "ERROR: Directory $dir is not writable"
        exit 1
    fi
done

log "Starting Thales trading bot..."

# Store PID for cleanup
echo $$ > /var/run/thales.pid

# Start the trading bot with config
exec /usr/local/bin/thales /etc/thales/config/config.json
