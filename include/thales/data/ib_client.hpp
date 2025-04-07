#pragma once

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <thales/core/portfolio.hpp>
#include <thales/data/market_data.hpp>
#include <thales/utils/config.hpp>
#include <thales/data/ib_wrapper.hpp>
#include <unordered_map>
#include <vector>

// Forward declarations for IB API classes
namespace IB {
class EClient;
class EWrapper;
class EReaderOSSignal;
class EReader;
}  // namespace IB

namespace thales {
namespace data {

/**
 * @class IBClient
 * @brief Client for interacting with Interactive Brokers API.
 *
 * This class provides an interface to the Interactive Brokers API
 * for retrieving market data, placing orders, and managing positions.
 */
class IBClient {
   public:
    // Friend declaration to allow IBWrapper to access private methods
    friend class IBWrapper;
    /**
     * @brief Constructor
     * @param config The configuration for the IB client
     */
    explicit IBClient(const utils::Config& config);

    /**
     * @brief Destructor
     */
    ~IBClient();

    /**
     * @brief Connect to the Interactive Brokers API
     * @return true if the connection was successful, false otherwise
     */
    bool connect();

    /**
     * @brief Disconnect from the Interactive Brokers API
     */
    void disconnect();

    /**
     * @brief Check if the client is connected
     * @return true if the client is connected, false otherwise
     */
    bool is_connected() const;

    /**
     * @brief Subscribe to market data for a symbol
     * @param symbol The symbol to subscribe to
     * @return true if the subscription was successful, false otherwise
     */
    bool subscribe_market_data(const std::string& symbol);

    /**
     * @brief Unsubscribe from market data for a symbol
     * @param symbol The symbol to unsubscribe from
     * @return true if the unsubscription was successful, false otherwise
     */
    bool unsubscribe_market_data(const std::string& symbol);

    /**
     * @brief Get the latest market data for a symbol
     * @param symbol The symbol to get data for
     * @return The latest market data for the symbol
     */
    MarketData get_latest_market_data(const std::string& symbol) const;

    /**
     * @brief Get historical market data for a symbol
     * @param symbol The symbol to get data for
     * @param startTime The start time for the historical data
     * @param endTime The end time for the historical data
     * @param interval The interval for the historical data (e.g., "1min",
     * "1hour", "1day")
     * @return A vector of historical market data
     */
    std::vector<MarketData> getHistoricalMarketData(
        const std::string& symbol, const std::string& startTime,
        const std::string& endTime, const std::string& interval) const;

    /**
     * @brief Get option chain data for a symbol
     * @param symbol The underlying symbol
     * @param expirationDate The expiration date (empty for all expirations)
     * @return A map of option data, keyed by option symbol
     */
    std::unordered_map<std::string, OptionData> getOptionChain(
        const std::string& symbol,
        const std::string& expirationDate = "") const;

    /**
     * @brief Place an order
     * @param order The order to place
     * @return The order ID if successful, empty string otherwise
     */
    std::string place_order(const core::Order& order);

    /**
     * @brief Cancel an order
     * @param orderId The order ID to cancel
     * @return true if the cancellation was successful, false otherwise
     */
    bool cancel_order(const std::string& orderId);

    /**
     * @brief Get all positions
     * @return A vector of positions
     */
    std::vector<core::Position> get_positions() const;

    /**
     * @brief Get all open orders
     * @return A vector of open orders
     */
    std::vector<core::Order> get_open_orders() const;

    /**
     * @brief Set a callback for market data updates
     * @param callback The callback function to set
     */
    void setMarketDataCallback(std::function<void(const MarketData&)> callback);

    /**
     * @brief Set a callback for order updates
     * @param callback The callback function to set
     */
    void setOrderUpdateCallback(
        std::function<void(const core::Order&)> callback);

    /**
     * @brief Set a callback for position updates
     * @param callback The callback function to set
     */
    void setPositionUpdateCallback(
        std::function<void(const core::Position&)> callback);

   private:
    // Configuration
    utils::Config config_;

    // IB API objects - using placeholders for now
    std::unique_ptr<thales::data::IBWrapper> wrapper_;
    std::unique_ptr<bool> client_;  // Placeholder for IB::EClient
    std::unique_ptr<int> signal_;   // Placeholder for IB::EReaderOSSignal
    std::unique_ptr<bool> reader_;  // Placeholder for IB::EReader
    std::thread message_processing_thread_;

    // Connection state
    bool connected_;

    // Market data cache
    std::unordered_map<std::string, MarketData> latestMarketData_;

    // Callbacks
    std::function<void(const MarketData&)> marketDataCallback_;
    std::function<void(const core::Order&)> orderUpdateCallback_;
    std::function<void(const core::Position&)> positionUpdateCallback_;

    // Request IDs
    int nextRequestId_;

    // Methods that need to be accessible to IBWrapper
    int getNextRequestId();
    void setNextRequestId(int id);
    void setManagedAccounts(const std::string& accounts);
    void processMessages();
    
    // Connection helper methods
    bool initializeWrapper();
    bool initializeSignal();
    bool initializeClient();
    bool connectToTWS(const std::string& host, int port, int client_id);
    bool startReaderThread();
    bool startMessageProcessingThread();
    bool requestAccountUpdates();
    void stopReaderThread();
    void disconnectFromTWS();
    void cleanupResources();
};

}  // namespace data
}  // namespace thales
