#include <chrono>
#include <iomanip>
#include <sstream>
#include <thales/utils/db_logger.hpp>
#include <thales/utils/logger.hpp>
#include <vector>

// Include libpqxx for PostgreSQL connectivity
#include <pqxx/pqxx>

namespace thales {
namespace utils {

// Initialize the static instance
std::unique_ptr<DbLogger> DbLogger::instance_ = nullptr;

DbLogger::DbLogger()
    : db_host_("localhost"),
      db_port_(5432),
      db_name_("thales"),
      db_user_("thales_user"),
      db_password_(""),
      max_queue_size_(10000),
      batch_size_(100),
      connected_(false),
      running_(false),
      failed_count_(0) {}

DbLogger::~DbLogger() { shutdown(); }

DbLogger& DbLogger::get_instance() {
    if (!instance_) {
        initialize();
    }
    return *instance_;
}

bool DbLogger::initialize(const std::string& dbHost, int dbPort,
                          const std::string& dbName, const std::string& dbUser,
                          const std::string& dbPassword, size_t maxQueueSize,
                          size_t batchSize) {
    if (!instance_) {
        instance_ = std::unique_ptr<DbLogger>(new DbLogger());
    }

    auto& logger = Logger::get_instance();
    logger.info("Initializing database logger");

    instance_->db_host_ = dbHost;
    instance_->db_port_ = dbPort;
    instance_->db_name_ = dbName;
    instance_->db_user_ = dbUser;
    instance_->db_password_ = dbPassword;
    instance_->max_queue_size_ = maxQueueSize;
    instance_->batch_size_ = batchSize;

    // Connect to the database
    if (!instance_->connect()) {
        logger.error("Failed to connect to the database");
        return false;
    }

    // Create tables if they don't exist
    if (!instance_->create_tables_if_not_exist()) {
        logger.error("Failed to create database tables");
        instance_->disconnect();
        return false;
    }

    // Start the worker thread
    instance_->running_ = true;
    instance_->worker_thread_ =
        std::thread(&DbLogger::worker_thread, instance_.get());

    logger.info("Database logger initialized successfully");
    return true;
}

bool DbLogger::connect() {
    std::lock_guard<std::mutex> lock(mutex_);

    auto& logger = thales::utils::Logger::get_instance();

    try {
        // Create a connection string
        std::string connectionString = get_connection_string();

        // Try to connect to the specified database
        try {
            auto conn = std::make_unique<pqxx::connection>(connectionString);

            if (conn->is_open()) {
                logger.info("Connected to PostgreSQL database: " + db_name_);
                connected_ = true;

                // Create tables if they don't exist
                if (!create_tables_if_not_exist()) {
                    logger.error("Failed to create database tables");
                    connected_ = false;
                    return false;
                }

                return true;
            }
        } catch (const pqxx::broken_connection& e) {
            // Database might not exist, try to create it
            logger.warning("Could not connect to database: " +
                           std::string(e.what()));
            logger.info("Attempting to create database...");

            // Connect to the 'postgres' database to create our database
            std::string postgresConnectionString =
                "host=" + db_host_ + " " + "port=" + std::to_string(db_port_) +
                " " + "dbname=postgres " + "user=" + db_user_ + " ";

            if (!db_password_.empty()) {
                postgresConnectionString += "password=" + db_password_ + " ";
            }

            postgresConnectionString += "connect_timeout=10";

            try {
                pqxx::connection postgresConn(postgresConnectionString);
                pqxx::work txn(postgresConn);

                // Check if database exists
                std::string checkDbQuery =
                    "SELECT 1 FROM pg_database WHERE datname = " +
                    txn.quote(db_name_);
                pqxx::result result = txn.exec(checkDbQuery);

                if (result.empty()) {
                    // Database doesn't exist, create it
                    std::string createDbQuery =
                        "CREATE DATABASE " + txn.esc(db_name_);
                    txn.exec(createDbQuery);
                    txn.commit();

                    logger.info("Database '" + db_name_ +
                                "' created successfully");

                    // Now try to connect to the new database
                    auto conn =
                        std::make_unique<pqxx::connection>(connectionString);

                    if (conn->is_open()) {
                        logger.info(
                            "Connected to newly created PostgreSQL database: " +
                            db_name_);
                        connected_ = true;

                        // Create tables
                        if (!create_tables_if_not_exist()) {
                            logger.error("Failed to create database tables");
                            connected_ = false;
                            return false;
                        }

                        return true;
                    }
                } else {
                    // Database exists but we couldn't connect, might be a
                    // permission issue
                    logger.error(
                        "Database '" + db_name_ +
                        "' exists but connection failed. Check permissions.");
                }
            } catch (const std::exception& e) {
                logger.error("Failed to create database: " +
                             std::string(e.what()));
                logger.info(
                    "You may need to run the setup script with superuser "
                    "privileges:");
                logger.info("  ./scripts/setup_postgres.sh");
            }
        }

        logger.error("Failed to connect to PostgreSQL database: " + db_name_);
        connected_ = false;
        return false;
    } catch (const std::exception& e) {
        logger.error("Exception while connecting to database: " +
                     std::string(e.what()));
        connected_ = false;
        return false;
    }

    return false;
}

void DbLogger::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    connected_ = false;
}

std::string DbLogger::get_connection_string() const {
    std::stringstream ss;
    ss << "host=" << db_host_ << " ";
    ss << "port=" << db_port_ << " ";
    ss << "dbname=" << db_name_ << " ";
    ss << "user=" << db_user_ << " ";
    if (!db_password_.empty()) {
        ss << "password=" << db_password_ << " ";
    }
    ss << "connect_timeout=10";
    return ss.str();
}

bool DbLogger::create_tables_if_not_exist() {
    std::lock_guard<std::mutex> lock(mutex_);

    auto& logger = Logger::get_instance();

    try {
        // Create a connection
        pqxx::connection conn(get_connection_string());

        // Create a transaction
        pqxx::work txn(conn);

        // Create the trade_executions table if it doesn't exist
        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS trade_executions (
                id SERIAL PRIMARY KEY,
                timestamp TIMESTAMP WITH TIME ZONE NOT NULL,
                strategy_name VARCHAR(100) NOT NULL,
                symbol VARCHAR(20) NOT NULL,
                order_id VARCHAR(50) NOT NULL,
                execution_id VARCHAR(50) NOT NULL,
                side VARCHAR(10) NOT NULL,
                quantity NUMERIC NOT NULL,
                price NUMERIC NOT NULL,
                commission NUMERIC,
                total_value NUMERIC NOT NULL,
                execution_time TIMESTAMP WITH TIME ZONE NOT NULL,
                account_id VARCHAR(50) NOT NULL,
                exchange VARCHAR(50),
                order_type VARCHAR(50),
                is_option BOOLEAN DEFAULT FALSE,
                option_data JSONB,
                additional_data JSONB
            )
        )");

        // Create indexes for faster queries
        txn.exec(
            "CREATE INDEX IF NOT EXISTS idx_trade_executions_timestamp ON "
            "trade_executions(timestamp)");
        txn.exec(
            "CREATE INDEX IF NOT EXISTS idx_trade_executions_symbol ON "
            "trade_executions(symbol)");
        txn.exec(
            "CREATE INDEX IF NOT EXISTS idx_trade_executions_strategy ON "
            "trade_executions(strategy_name)");

        // Commit the transaction
        txn.commit();

        logger.info("Database tables created successfully");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception while creating database tables: " +
                     std::string(e.what()));
        return false;
    }
}

bool DbLogger::log_trade_execution(
    const std::string& strategy_name, const std::string& symbol,
    const std::string& order_id, const std::string& execution_id,
    const std::string& side, double quantity, double price, double commission,
    double total_value, const std::string& execution_time,
    const std::string& account_id, const std::string& exchange,
    const std::string& order_type, bool is_option,
    const std::string& option_data, const std::string& additional_data) {
    auto& logger = Logger::get_instance();

    // Create a log entry
    TradeExecutionLog log;
    log.strategy_name = strategy_name;
    log.symbol = symbol;
    log.order_id = order_id;
    log.execution_id = execution_id;
    log.side = side;
    log.quantity = quantity;
    log.price = price;
    log.commission = commission;
    log.total_value = total_value;
    log.execution_time = execution_time;
    log.account_id = account_id;
    log.exchange = exchange;
    log.order_type = order_type;
    log.is_option = is_option;
    log.option_data = option_data;
    log.additional_data = additional_data;
    log.timestamp = get_current_timestamp();

    // Add the log to the queue
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);

        // Check if the queue is full
        if (log_queue_.size() >= max_queue_size_) {
            logger.warning("Database log queue is full, dropping oldest log");
            log_queue_.pop();
        }

        log_queue_.push(log);
    }

    // Notify the worker thread
    queue_condition_.notify_one();

    return true;
}

void DbLogger::worker_thread() {
    auto& logger = Logger::get_instance();
    logger.info("Database logger worker thread started");

    while (running_) {
        try {
            // Process logs in the queue
            process_logs();

            // Sleep for a short time
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } catch (const std::exception& e) {
            logger.error("Exception in database logger worker thread: " +
                         std::string(e.what()));
        }
    }

    // Process any remaining logs
    try {
        process_logs();
    } catch (const std::exception& e) {
        logger.error("Exception while processing remaining logs: " +
                     std::string(e.what()));
    }

    logger.info("Database logger worker thread stopped");
}

void DbLogger::process_logs() {
    auto& logger = Logger::get_instance();

    // Check if there are logs to process
    std::vector<TradeExecutionLog> logs;

    {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        // Wait for logs or shutdown signal
        queue_condition_.wait_for(lock, std::chrono::seconds(1), [this] {
            return !log_queue_.empty() || !running_;
        });

        // If there are no logs and we're shutting down, return
        if (log_queue_.empty()) {
            return;
        }

        // Get a batch of logs
        size_t batch_size = std::min(batch_size_, log_queue_.size());
        logs.reserve(batch_size);

        for (size_t i = 0; i < batch_size; ++i) {
            logs.push_back(log_queue_.front());
            log_queue_.pop();
        }
    }

    // Insert the logs
    if (!logs.empty()) {
        if (!insert_log_batch(logs)) {
            logger.error("Failed to insert log batch");

            // Put the logs back in the queue
            std::lock_guard<std::mutex> lock(queue_mutex_);
            for (auto it = logs.rbegin(); it != logs.rend(); ++it) {
                if (log_queue_.size() < max_queue_size_) {
                    log_queue_.push(*it);
                } else {
                    failed_count_++;
                }
            }
        }
    }
}

bool DbLogger::insert_log_batch(const std::vector<TradeExecutionLog>& logs) {
    if (logs.empty()) {
        return true;
    }

    auto& logger = Logger::get_instance();

    try {
        // Check if we're connected
        if (!connected_) {
            if (!connect()) {
                return false;
            }
        }

        // Create a connection
        pqxx::connection conn(get_connection_string());

        // Create a transaction
        pqxx::work txn(conn);

        // Prepare the statement
        pqxx::prepare::declaration stmt = txn.conn().prepare(
            "insert_trade_execution",
            "INSERT INTO trade_executions ("
            "timestamp, strategy_name, symbol, order_id, execution_id, "
            "side, quantity, price, commission, total_value, "
            "execution_time, account_id, exchange, order_type, "
            "is_option, option_data, additional_data"
            ") VALUES ("
            "$1, $2, $3, $4, $5, "
            "$6, $7, $8, $9, $10, "
            "$11, $12, $13, $14, "
            "$15, $16, $17"
            ")");

        // Insert each log
        for (const auto& log : logs) {
            txn.exec_prepared("insert_trade_execution", log.timestamp,
                              log.strategy_name, log.symbol, log.order_id,
                              log.execution_id, log.side, log.quantity,
                              log.price, log.commission, log.total_value,
                              log.execution_time, log.account_id, log.exchange,
                              log.order_type, log.is_option, log.option_data,
                              log.additional_data);
        }

        // Commit the transaction
        txn.commit();

        return true;
    } catch (const std::exception& e) {
        logger.error("Exception while inserting log batch: " +
                     std::string(e.what()));
        connected_ = false;
        return false;
    }
}

void DbLogger::shutdown() {
    auto& logger = Logger::get_instance();
    logger.info("Shutting down database logger");

    // Stop the worker thread
    running_ = false;
    queue_condition_.notify_all();

    // Wait for the worker thread to finish
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }

    // Disconnect from the database
    disconnect();

    logger.info("Database logger shut down");
}

bool DbLogger::is_connected() const { return connected_; }

size_t DbLogger::get_queue_size() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return log_queue_.size();
}

size_t DbLogger::get_failed_count() const { return failed_count_; }

std::string DbLogger::get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time), "%Y-%m-%d %H:%M:%S");

    return ss.str();
}

}  // namespace utils
}  // namespace thales
