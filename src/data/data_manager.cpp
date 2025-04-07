#include <thales/data/data_manager.hpp>
#include <thales/utils/logger.hpp>

namespace thales {
namespace data {

DataManager::DataManager(const utils::Config& config) : config_(config) {}

DataManager::~DataManager() {
    // Clean up resources
    for (const auto& symbol : subscribed_symbols_) {
        unsubscribe_market_data(symbol);
    }
}

bool DataManager::initialize() {
    auto& logger = utils::Logger::get_instance();
    logger.info("Initializing data manager...");

    try {
#ifdef ENABLE_IB_CLIENT
        // Initialize IB client if enabled
        ib_client_ = std::make_unique<IBClient>(config_);
        if (!ib_client_->connect()) {
            logger.error("Failed to connect to IB client");
            return false;
        }
        logger.info("IB client connected successfully");
#else
        logger.info("IB client disabled (not compiled with ENABLE_IB_CLIENT)");
#endif

        // Initialize market data cache
        latest_market_data_.clear();

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

bool DataManager::subscribe_market_data(const std::string& symbol) {
    auto& logger = utils::Logger::get_instance();
    logger.info("Subscribing to market data for " + symbol);

    // Check if already subscribed
    if (std::find(subscribed_symbols_.begin(), subscribed_symbols_.end(),
                  symbol) != subscribed_symbols_.end()) {
        logger.info("Already subscribed to " + symbol);
        return true;
    }

#ifdef ENABLE_IB_CLIENT
    // Subscribe through IB client
    if (ib_client_ && ib_client_->subscribe_market_data(symbol)) {
        subscribed_symbols_.push_back(symbol);
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

bool DataManager::unsubscribe_market_data(const std::string& symbol) {
    auto& logger = utils::Logger::get_instance();
    logger.info("Unsubscribing from market data for " + symbol);

    auto it = std::find(subscribed_symbols_.begin(), subscribed_symbols_.end(),
                        symbol);
    if (it == subscribed_symbols_.end()) {
        logger.info("Not subscribed to " + symbol);
        return true;
    }

#ifdef ENABLE_IB_CLIENT
    // Unsubscribe through IB client
    if (ib_client_ && ib_client_->unsubscribe_market_data(symbol)) {
        subscribed_symbols_.erase(it);
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

MarketData DataManager::get_latest_market_data(
    const std::string& symbol) const {
    auto it = latest_market_data_.find(symbol);
    if (it != latest_market_data_.end()) {
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

std::vector<MarketData> DataManager::get_historical_market_data(
    [[maybe_unused]] const std::string& symbol, 
    [[maybe_unused]] const std::string& startTime,
    [[maybe_unused]] const std::string& endTime, 
    [[maybe_unused]] const std::string& interval) const {
    // For the stub implementation, return an empty vector
    return {};
}

std::unordered_map<std::string, OptionData> DataManager::get_option_chain(
    [[maybe_unused]] const std::string& symbol, 
    [[maybe_unused]] const std::string& expirationDate) const {
    // For the stub implementation, return an empty map
    return {};
}

void DataManager::process_market_data(const MarketData& data) {
    cache_market_data(data);

    // In a real implementation, this would notify subscribers, update
    // analytics, etc.
}

bool DataManager::connect_to_data_sources() {
#ifdef ENABLE_IB_CLIENT
    // Connect to IB
    if (ib_client_) {
        return ib_client_->connect();
    }
#endif

    // For the stub implementation, always return true
    return true;
}

void DataManager::cache_market_data(const MarketData& data) {
    latest_market_data_[data.symbol] = data;
}

}  // namespace data
}  // namespace thales
