// SPDX-License-Identifier: MIT

// Standard library includes
#include <algorithm>
#include <mutex>

// Project includes
#include "thales/data/data_manager.hpp"
#include "thales/utils/logger.hpp"

namespace thales {
namespace data {

namespace {
// Thread-safe access to shared data
std::mutex g_market_data_mutex;
}  // namespace

DataManager::DataManager(const utils::Config &config)
    : config_(config), ib_client_(nullptr) {}

DataManager::~DataManager() {
    try {
        // Clean up market data subscriptions
        for (const auto &symbol : subscribed_symbols_) {
            unsubscribe_market_data(symbol);
        }

        // Clear caches
        {
            std::lock_guard<std::mutex> lock(g_market_data_mutex);
            latest_market_data_.clear();
            subscribed_symbols_.clear();
        }
    } catch (const std::exception &e) {
        auto &logger = utils::Logger::get_instance();
        logger.error("Error during DataManager cleanup: " +
                     std::string(e.what()));
    }
}

bool DataManager::initialize() {
    auto &logger = utils::Logger::get_instance();
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
    } catch (const std::exception &e) {
        logger.error("Exception during data manager initialization: " +
                     std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception during data manager initialization");
        return false;
    }
}

bool DataManager::subscribe_market_data(const std::string &symbol) {
    auto &logger = utils::Logger::get_instance();
    logger.info("Subscribing to market data for " + symbol);

    try {
        // Thread-safe check for existing subscription
        {
            std::lock_guard<std::mutex> lock(g_market_data_mutex);
            if (std::find(subscribed_symbols_.begin(),
                          subscribed_symbols_.end(),
                          symbol) != subscribed_symbols_.end()) {
                logger.info("Already subscribed to " + symbol);
                return true;
            }
        }

#ifdef ENABLE_IB_CLIENT
        if (!ib_client_) {
            logger.error("IB client not initialized");
            return false;
        }

        if (ib_client_->subscribe_market_data(symbol)) {
            std::lock_guard<std::mutex> lock(g_market_data_mutex);
            subscribed_symbols_.push_back(symbol);
            logger.info("Successfully subscribed to " + symbol);
            return true;
        }

        logger.error("Failed to subscribe to " + symbol);
        return false;
#else
        // Create dummy market data for testing
        MarketData dummy_data;
        dummy_data.symbol = symbol;
        dummy_data.price = 100.0;
        dummy_data.volume = 1000;
        dummy_data.timestamp = "2023-04-01T12:00:00Z";

        {
            std::lock_guard<std::mutex> lock(g_market_data_mutex);
            latest_market_data_[symbol] = dummy_data;
            subscribed_symbols_.push_back(symbol);
        }

        logger.info("Created dummy market data for " + symbol);
        return true;
#endif
    } catch (const std::exception &e) {
        logger.error("Error subscribing to market data: " +
                     std::string(e.what()));
        return false;
    }
}

bool DataManager::unsubscribe_market_data(const std::string &symbol) {
    auto &logger = utils::Logger::get_instance();
    logger.info("Unsubscribing from market data for " + symbol);

    try {
        std::vector<std::string>::iterator it;
        {
            std::lock_guard<std::mutex> lock(g_market_data_mutex);
            it = std::find(subscribed_symbols_.begin(),
                           subscribed_symbols_.end(), symbol);
            if (it == subscribed_symbols_.end()) {
                logger.info("Not subscribed to " + symbol);
                return true;
            }
        }

#ifdef ENABLE_IB_CLIENT
        if (!ib_client_) {
            logger.error("IB client not initialized");
            return false;
        }

        if (ib_client_->unsubscribe_market_data(symbol)) {
            std::lock_guard<std::mutex> lock(g_market_data_mutex);
            subscribed_symbols_.erase(it);
            latest_market_data_.erase(symbol);
            logger.info("Successfully unsubscribed from " + symbol);
            return true;
        }

        logger.error("Failed to unsubscribe from " + symbol);
        return false;
#else
        {
            std::lock_guard<std::mutex> lock(g_market_data_mutex);
            subscribed_symbols_.erase(it);
            latest_market_data_.erase(symbol);
        }
        logger.info("Removed dummy subscription for " + symbol);
        return true;
#endif
    } catch (const std::exception &e) {
        logger.error("Error unsubscribing from market data: " +
                     std::string(e.what()));
        return false;
    }
}

MarketData DataManager::get_latest_market_data(
    const std::string &symbol) const {
    try {
        std::lock_guard<std::mutex> lock(g_market_data_mutex);
        auto it = latest_market_data_.find(symbol);
        if (it != latest_market_data_.end()) {
            return it->second;
        }

        // Return empty market data if not found
        MarketData empty_data;
        empty_data.symbol = symbol;
        empty_data.price = 0.0;
        empty_data.volume = 0;
        empty_data.timestamp = "";
        return empty_data;
    } catch (const std::exception &e) {
        auto &logger = utils::Logger::get_instance();
        logger.error("Error retrieving market data: " + std::string(e.what()));
        MarketData error_data;
        error_data.symbol = symbol;
        return error_data;
    }
}

struct HistoricalDataRequest {
    std::string symbol;
    std::string start_time;
    std::string end_time;
    std::string interval;
};

std::vector<MarketData> DataManager::get_historical_market_data(
    const std::string &symbol, const std::string &start_time,
    const std::string &end_time, const std::string &interval) const {
    const HistoricalDataRequest request{.symbol = symbol,
                                        .start_time = start_time,
                                        .end_time = end_time,
                                        .interval = interval};

    // TODO: Implement historical data retrieval
    // For now, return empty vector
    return {};
}

struct OptionChainRequest {
    std::string symbol;
    std::string expiration_date;
};

std::unordered_map<std::string, OptionData> DataManager::get_option_chain(
    const std::string &symbol, const std::string &expiration_date) const {
    const OptionChainRequest request{.symbol = symbol,
                                     .expiration_date = expiration_date};

    // TODO: Implement option chain retrieval
    // For now, return empty map
    return {};
}

void DataManager::process_market_data(const MarketData &data) {
    try {
        // Cache the new data
        cache_market_data(data);

        // TODO: In a real implementation:
        // - Notify data subscribers
        // - Update analytics
        // - Log significant price moves
        // - Check for alerts/triggers
    } catch (const std::exception &e) {
        auto &logger = utils::Logger::get_instance();
        logger.error("Error processing market data: " + std::string(e.what()));
    }
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

void DataManager::cache_market_data(const MarketData &data) {
    std::lock_guard<std::mutex> lock(g_market_data_mutex);
    latest_market_data_[data.symbol] = data;
}

}  // namespace data
}  // namespace thales
