#include <thales/data/data_manager.hpp>
#include <thales/utils/logger.hpp>

namespace thales {
namespace data {

DataManager::DataManager(const utils::Config& config) : config_(config) {}

DataManager::~DataManager() {
    // Clean up resources
    for (const auto& symbol : subscribedSymbols_) {
        unsubscribeMarketData(symbol);
    }
}

bool DataManager::initialize() {
    auto& logger = utils::Logger::get_instance();
    logger.info("Initializing data manager...");

    try {
#ifdef ENABLE_IB_CLIENT
        // Initialize IB client if enabled
        ibClient_ = std::make_unique<IBClient>(config_);
        if (!ibClient_->connect()) {
            logger.error("Failed to connect to IB client");
            return false;
        }
        logger.info("IB client connected successfully");
#else
        logger.info("IB client disabled (not compiled with ENABLE_IB_CLIENT)");
#endif

        // Initialize market data cache
        latestMarketData_.clear();

        logger.info("Data manager initialized successfully");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception during data manager initialization: " +
                     std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception during data manager initialization");
        return false;
    }
}

bool DataManager::subscribeMarketData(const std::string& symbol) {
    auto& logger = utils::Logger::get_instance();
    logger.info("Subscribing to market data for " + symbol);

    // Check if already subscribed
    if (std::find(subscribedSymbols_.begin(), subscribedSymbols_.end(),
                  symbol) != subscribedSymbols_.end()) {
        logger.info("Already subscribed to " + symbol);
        return true;
    }

#ifdef ENABLE_IB_CLIENT
    // Subscribe through IB client
    if (ibClient_ && ibClient_->subscribeMarketData(symbol)) {
        subscribedSymbols_.push_back(symbol);
        logger.info("Successfully subscribed to " + symbol);
        return true;
    } else {
        logger.error("Failed to subscribe to " + symbol);
        return false;
    }
#else
    // Create a dummy market data entry for testing
    MarketData dummyData;
    dummyData.symbol = symbol;
    dummyData.price = 100.0;  // Default dummy price
    dummyData.volume = 1000;
    dummyData.timestamp = "2023-04-01T12:00:00Z";
    latestMarketData_[symbol] = dummyData;

    subscribedSymbols_.push_back(symbol);
    logger.info("Created dummy market data for " + symbol);
    return true;
#endif
}

bool DataManager::unsubscribeMarketData(const std::string& symbol) {
    auto& logger = utils::Logger::get_instance();
    logger.info("Unsubscribing from market data for " + symbol);

    auto it =
        std::find(subscribedSymbols_.begin(), subscribedSymbols_.end(), symbol);
    if (it == subscribedSymbols_.end()) {
        logger.info("Not subscribed to " + symbol);
        return true;
    }

#ifdef ENABLE_IB_CLIENT
    // Unsubscribe through IB client
    if (ibClient_ && ibClient_->unsubscribeMarketData(symbol)) {
        subscribedSymbols_.erase(it);
        logger.info("Successfully unsubscribed from " + symbol);
        return true;
    } else {
        logger.error("Failed to unsubscribe from " + symbol);
        return false;
    }
#else
    // Remove from subscribed symbols
    subscribedSymbols_.erase(it);
    logger.info("Removed dummy subscription for " + symbol);
    return true;
#endif
}

MarketData DataManager::getLatestMarketData(const std::string& symbol) const {
    auto it = latestMarketData_.find(symbol);
    if (it != latestMarketData_.end()) {
        return it->second;
    }

    // Return empty market data if not found
    MarketData emptyData;
    emptyData.symbol = symbol;
    emptyData.price = 0.0;
    emptyData.volume = 0;
    emptyData.timestamp = "";

    return emptyData;
}

std::vector<MarketData> DataManager::getHistoricalMarketData(
    const std::string& symbol, const std::string& startTime,
    const std::string& endTime, const std::string& interval) const {
    // For the stub implementation, return an empty vector
    return {};
}

std::unordered_map<std::string, OptionData> DataManager::getOptionChain(
    const std::string& symbol, const std::string& expirationDate) const {
    // For the stub implementation, return an empty map
    return {};
}

void DataManager::processMarketData(const MarketData& data) {
    cacheMarketData(data);

    // In a real implementation, this would notify subscribers, update
    // analytics, etc.
}

bool DataManager::connectToDataSources() {
#ifdef ENABLE_IB_CLIENT
    // Connect to IB
    if (ibClient_) {
        return ibClient_->connect();
    }
#endif

    // For the stub implementation, always return true
    return true;
}

void DataManager::cacheMarketData(const MarketData& data) {
    latestMarketData_[data.symbol] = data;
}

}  // namespace data
}  // namespace thales
