// SPDX-License-Identifier: MIT

#pragma once

// Standard library includes
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

// Project includes
#include <thales/core/portfolio.hpp>
#include <thales/data/ib_wrapper.hpp>
#include <thales/data/market_data.hpp>
#include <thales/utils/config.hpp>

// Forward declarations for IB API classes
namespace IB {
class EClient;
class EWrapper;
class EReaderOSSignal;
class EReader;
} // namespace IB

namespace thales {
namespace data {

/// Interactive Brokers API client interface
///
/// Provides high-level access to IB TWS/Gateway functionality:
/// - Market data operations (real-time and historical)
/// - Order management (placement, cancellation, status)
/// - Position tracking
/// - Options chain data
///
/// Manages connection lifecycle and message processing.
/// Thread-safe interface to TWS/Gateway.
class IBClient {
  public:
    friend class IBWrapper; // Allows wrapper to access message handlers

    /// Parameters for historical market data requests
    struct HistoricalDataParams {
        std::string symbol;     ///< Trading symbol
        std::string start_time; ///< Start of data range
        std::string end_time;   ///< End of data range
        std::string interval;   ///< Bar size (e.g., "1 min")
    };

    /// Parameters for option chain requests
    struct OptionChainParams {
        std::string symbol;          ///< Underlying symbol
        std::string expiration_date; ///< Option expiration date
    };

    /// Parameters for TWS/Gateway connection
    struct ConnectionParams {
        std::string host; ///< TWS/Gateway hostname
        int port;         ///< Connection port
        int client_id;    ///< Unique client identifier
    };

    /// Construct a new IBClient instance
    /// @param config Client configuration parameters
    explicit IBClient(const utils::Config &config);

    /// @brief Destructor
    ~IBClient();

    /// Initialize connection to TWS/Gateway
    /// @return true if connection succeeds
    bool connect();

    /// @brief Disconnect from the Interactive Brokers API
    void disconnect();

    /// @brief Check if the client is connected
    /// @return true if the client is connected, false otherwise
    bool is_connected() const;

    /// Start real-time market data feed
    /// @param symbol Trading symbol to subscribe
    /// @return true if subscription succeeds
    bool subscribe_market_data(const std::string &symbol);

    /// Stop real-time market data feed
    /// @param symbol Trading symbol to unsubscribe
    /// @return true if unsubscription succeeds
    bool unsubscribe_market_data(const std::string &symbol);

    /// Retrieve most recent market data
    /// @param symbol Trading symbol to query
    /// @return Latest market data snapshot
    MarketData get_latest_market_data(const std::string &symbol) const;

    /// Fetch historical market data
    /// @param params Historical data request parameters
    /// @return Vector of historical data points
    std::vector<MarketData>
    get_historical_market_data(const HistoricalDataParams &params) const;

    /// Retrieve option chain information
    /// @param params Option chain request parameters
    /// @return Map of option data by symbol
    std::unordered_map<std::string, OptionData>
    get_option_chain(const OptionChainParams &params) const;

    /// Submit new order to TWS/Gateway
    /// @param order Order to place
    /// @return Order ID if successful, empty string on failure
    std::string place_order(const core::Order &order);

    /// Cancel existing order
    /// @param order_id ID of order to cancel
    /// @return true if cancellation succeeds
    bool cancel_order(const std::string &orderId);

    /// Retrieve all current positions
    /// @return Vector of active positions
    std::vector<core::Position> get_positions() const;

    /// Retrieve all open orders
    /// @return Vector of pending orders
    std::vector<core::Order> get_open_orders() const;

    /// Register market data update handler
    /// @param callback Function to handle updates
    void
    set_market_data_callback(std::function<void(const MarketData &)> callback);

    /// Register order status update handler
    /// @param callback Function to handle updates
    void set_order_update_callback(
        std::function<void(const core::Order &)> callback);

    /// Register position update handler
    /// @param callback Function to handle updates
    void set_position_update_callback(
        std::function<void(const core::Position &)> callback);

  private:
    // Core configuration
    utils::Config config_; ///< Client configuration

    // IB API components
    std::unique_ptr<IBWrapper> wrapper_;    ///< Message handler wrapper
    std::unique_ptr<bool> client_;          ///< IB::EClient placeholder
    std::unique_ptr<int> signal_;           ///< IB::EReaderOSSignal placeholder
    std::unique_ptr<bool> reader_;          ///< IB::EReader placeholder
    std::thread message_processing_thread_; ///< Message processing thread

    // State management
    bool connected_;      ///< Connection state flag
    int next_request_id_; ///< Request sequence counter

    // Data caches
    std::unordered_map<std::string, MarketData>
        latest_market_data_; ///< Market data cache

    // Callback handlers
    std::function<void(const MarketData &)> market_data_callback_;
    std::function<void(const core::Order &)> order_update_callback_;
    std::function<void(const core::Position &)> position_update_callback_;

    // Connection management
    bool connect_to_tws(const ConnectionParams &params);
    bool start_reader_thread();
    bool start_message_processing_thread();
    bool request_account_updates();
    void stop_reader_thread();
    void disconnect_from_tws();
    void cleanup_resources();

    // Core message handling
    void process_messages();
    int get_next_request_id();
    void set_next_request_id(int id);
    void set_managed_accounts(const std::string &accounts);

    // Initialization helpers
    bool initialize_wrapper();
    bool initialize_signal();
    bool initialize_client();
};

} // namespace data
} // namespace thales
