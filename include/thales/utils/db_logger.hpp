#ifndef THALES_UTILS_DB_LOGGER_HPP
#define THALES_UTILS_DB_LOGGER_HPP

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace thales {
namespace utils {

/**
 * @class DbLogger
 * @brief Database logger class for the trading bot.
 *
 * This class provides thread-safe logging functionality to a PostgreSQL
 * database. It implements a singleton pattern and uses asynchronous logging
 * with a queue to minimize impact on trading performance.
 */
class DbLogger {
   public:
    /**
     * @brief Get the singleton instance of the database logger
     * @return Reference to the database logger instance
     */
    static DbLogger& get_instance();

    /**
     * @brief Initialize the database logger
     * @param dbHost Database host
     * @param dbPort Database port
     * @param dbName Database name
     * @param dbUser Database user
     * @param dbPassword Database password
     * @param maxQueueSize Maximum size of the log queue
     * @param batchSize Number of logs to insert in a batch
     * @return true if initialization was successful, false otherwise
     */
    static bool initialize(const std::string& dbHost = "localhost",
                           int dbPort = 5432,
                           const std::string& dbName = "thales",
                           const std::string& dbUser = "thales_user",
                           const std::string& dbPassword = "",
                           size_t maxQueueSize = 10000, size_t batchSize = 100);

    /**
     * @brief Log a trade execution to the database
     * @param strategyName Name of the strategy that generated the trade
     * @param ticker Trading ticker
     * @param orderId Order ID
     * @param executionId Execution ID
     * @param side Trade side (BUY or SELL)
     * @param quantity Trade quantity
     * @param price Trade price
     * @param commission Trade commission
     * @param totalValue Total value of the trade
     * @param executionTime Time of execution
     * @param accountId Account ID
     * @param exchange Exchange name
     * @param orderType Order type
     * @param isOption Whether the trade is for an option
     * @param optionData JSON string with option-specific data
     * @param additionalData JSON string with additional data
     * @return true if the log was queued successfully, false otherwise
     */
    bool log_trade_execution(
        const std::string& strategy_name, const std::string& ticker,
        const std::string& order_id, const std::string& execution_id,
        const std::string& side, double quantity, double price,
        double commission, double total_value,
        const std::string& execution_time, const std::string& account_id,
        const std::string& exchange, const std::string& order_type,
        bool is_option = false, const std::string& option_data = "{}",
        const std::string& additional_data = "{}");

    /**
     * @brief Shutdown the database logger
     *
     * This method stops the worker thread and flushes any remaining logs.
     */
    void shutdown();

    /**
     * @brief Check if the database logger is connected
     * @return true if connected, false otherwise
     */
    bool is_connected() const;

    /**
     * @brief Get the number of logs in the queue
     * @return Number of logs in the queue
     */
    size_t get_queue_size() const;

    /**
     * @brief Get the number of failed log insertions
     * @return Number of failed log insertions
     */
    size_t get_failed_count() const;

   private:
    // Private constructor for singleton pattern
    DbLogger();

    // Destructor
    ~DbLogger();

    // Prevent copying and assignment
    DbLogger(const DbLogger&) = delete;
    DbLogger& operator=(const DbLogger&) = delete;

    // Structure to hold a trade execution log
    struct TradeExecutionLog {
        std::string strategy_name;
        std::string ticker;
        std::string order_id;
        std::string execution_id;
        std::string side;
        double quantity;
        double price;
        double commission;
        double total_value;
        std::string execution_time;
        std::string account_id;
        std::string exchange;
        std::string order_type;
        bool is_option;
        std::string option_data;
        std::string additional_data;
        std::string timestamp;
    };

    // Connect to the database
    bool connect();

    // Disconnect from the database
    void disconnect();

    // Get the connection string
    std::string get_connection_string() const;

    // Create the necessary tables if they don't exist
    bool create_tables_if_not_exist();

    // Worker thread function
    void worker_thread();

    // Process logs in the queue
    void process_logs();

    // Insert a batch of logs
    bool insert_log_batch(const std::vector<TradeExecutionLog>& logs);

    // Get the current timestamp
    static std::string get_current_timestamp();

    // Instance variables
    std::mutex mutex_;
    std::string db_host_;
    int db_port_;
    std::string db_name_;
    std::string db_user_;
    std::string db_password_;
    size_t max_queue_size_;
    size_t batch_size_;

    // Connection status
    std::atomic<bool> connected_;

    // Queue for logs
    std::queue<TradeExecutionLog> log_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_condition_;

    // Worker thread
    std::thread worker_thread_;
    std::atomic<bool> running_;

    // Statistics
    std::atomic<size_t> failed_count_;

    // Singleton instance
    static std::unique_ptr<DbLogger> instance_;
};

}  // namespace utils
}  // namespace thales

#endif  // THALES_UTILS_DB_LOGGER_HPP
