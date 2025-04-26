// SPDX-License-Identifier: MIT

#pragma once

// Standard library includes
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Project includes
#include "thales/data/ib_client.hpp"
#include "thales/data/market_data.hpp"
#include "thales/utils/config.hpp"

namespace thales {
namespace data {

/// Market data management system
///
/// Provides centralized management of:
/// - Real-time market data subscriptions
/// - Historical data retrieval
/// - Option chain information
/// - Market data caching
///
/// Primary interface for all market data operations, abstracting
/// provider-specific details (e.g., Interactive Brokers integration).
class DataManager {
   public:
    /// Construct a new DataManager instance
    /// @param config Market data configuration parameters
    explicit DataManager(const utils::Config &config);

    /// @brief Destructor
    ~DataManager();

    /// Initialize market data connections and caching
    /// @return true if all systems initialized successfully
    bool initialize();

    /// Start real-time market data feed for symbol
    /// @param symbol Trading symbol to subscribe to
    /// @return true if subscription succeeded
    bool subscribe_market_data(const std::string &symbol);

    /// Stop real-time market data feed for symbol
    /// @param symbol Trading symbol to unsubscribe from
    /// @return true if unsubscription succeeded
    bool unsubscribe_market_data(const std::string &symbol);

    /// Retrieve most recent market data snapshot
    /// @param symbol Trading symbol to query
    /// @return Latest market data for the symbol
    MarketData get_latest_market_data(const std::string &symbol) const;

    /// Fetch historical market data
    /// @param symbol Trading symbol to query
    /// @param startTime Historical data start time
    /// @param endTime Historical data end time
    /// @param interval Bar interval (e.g., "1min", "1hour", "1day")
    /// @return Vector of historical market data points
    std::vector<MarketData> get_historical_market_data(
        const std::string &symbol, const std::string &startTime,
        const std::string &endTime, const std::string &interval) const;

    /// Retrieve option chain information
    /// @param symbol Underlying symbol to query
    /// @param expirationDate Specific expiry date (empty for all)
    /// @return Map of option data indexed by option symbol
    std::unordered_map<std::string, OptionData> get_option_chain(
        const std::string &symbol,
        const std::string &expirationDate = "") const;

    /// Handle incoming market data updates
    /// @param data New market data to process
    void process_market_data(const MarketData &data);

   private:
    utils::Config config_;                 ///< Data manager configuration
    std::unique_ptr<IBClient> ib_client_;  ///< Interactive Brokers connection

    // Data caches
    std::unordered_map<std::string, MarketData>
        latest_market_data_;                       ///< Real-time data cache
    std::vector<std::string> subscribed_symbols_;  ///< Active subscriptions

    /// Establish connections to market data providers
    /// @return true if all connections succeeded
    bool connect_to_data_sources();

    /// Update market data cache with new information
    /// @param data New market data to cache
    void cache_market_data(const MarketData &data);
};

}  // namespace data
}  // namespace thales
