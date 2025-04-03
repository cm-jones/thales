#pragma once

#include <memory>
#include <string>
#include <thales/data/ib_client.hpp>
#include <thales/data/market_data.hpp>
#include <thales/utils/config.hpp>
#include <unordered_map>
#include <vector>

namespace thales {
namespace data {

/**
 * @class DataManager
 * @brief Manages market data for the trading bot.
 *
 * This class is responsible for retrieving, storing, and providing
 * access to market data from various sources, such as Interactive Brokers.
 */
class DataManager {
   public:
    /**
     * @brief Constructor
     * @param config The configuration for the data manager
     */
    explicit DataManager(const utils::Config& config);

    /**
     * @brief Destructor
     */
    ~DataManager();

    /**
     * @brief Initialize the data manager
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Subscribe to market data for a symbol
     * @param symbol The symbol to subscribe to
     * @return true if the subscription was successful, false otherwise
     */
    bool subscribeMarketData(const std::string& symbol);

    /**
     * @brief Unsubscribe from market data for a symbol
     * @param symbol The symbol to unsubscribe from
     * @return true if the unsubscription was successful, false otherwise
     */
    bool unsubscribeMarketData(const std::string& symbol);

    /**
     * @brief Get the latest market data for a symbol
     * @param symbol The symbol to get data for
     * @return The latest market data for the symbol
     */
    MarketData getLatestMarketData(const std::string& symbol) const;

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
     * @brief Process incoming market data
     * @param data The market data to process
     */
    void processMarketData(const MarketData& data);

   private:
    // Configuration
    utils::Config config_;

    // Interactive Brokers client
    std::unique_ptr<IBClient> ibClient_;

    // Market data cache
    std::unordered_map<std::string, MarketData> latestMarketData_;

    // Subscribed symbols
    std::vector<std::string> subscribedSymbols_;

    // Private methods
    bool connectToDataSources();
    void cacheMarketData(const MarketData& data);
};

}  // namespace data
}  // namespace thales
