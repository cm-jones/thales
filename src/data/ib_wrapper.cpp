// SPDX-License-Identifier: MIT

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thales/data/ib_client.hpp>
#include <thales/data/ib_wrapper.hpp>
#include <thales/utils/logger.hpp>

namespace thales {
namespace data {

IBWrapper::IBWrapper(IBClient &client) : client_(client) {
    // Initialize any necessary state
}

IBWrapper::~IBWrapper() {
    // Clean up any resources
}

// Connection and server methods
void IBWrapper::connectAck() {
    auto &logger = utils::Logger::get_instance();
    logger.info("Connection to IB TWS/Gateway acknowledged");
}

void IBWrapper::connectionClosed() {
    auto &logger = utils::Logger::get_instance();
    logger.info("Connection to IB TWS/Gateway closed");

    // Notify the client that the connection was closed
    // This would typically be handled by the client's disconnect method
}

void IBWrapper::currentTime(long time) {
    auto &logger = utils::Logger::get_instance();

    // Convert time_t to string
    std::time_t t = static_cast<std::time_t>(time);
    std::tm *tm_info = std::localtime(&t);

    std::stringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");

    logger.debug("IB server time: " + ss.str());
}

void IBWrapper::error(int id, int errorCode, const std::string &errorString,
                      const std::string &advancedOrderRejectJson) {
    auto &logger = utils::Logger::get_instance();

    std::string message = "IB API Error " + std::to_string(errorCode) +
                          " for request " + std::to_string(id) + ": " +
                          errorString;

    if (!advancedOrderRejectJson.empty()) {
        message += " (Advanced order reject: " + advancedOrderRejectJson + ")";
    }

    // Log at appropriate level based on error code
    if (errorCode >= 1000 && errorCode < 2000) {
        // System errors
        logger.error(message);
    } else if (errorCode >= 2000 && errorCode < 3000) {
        // Warning messages
        logger.warning(message);
    } else if (errorCode >= 10000 && errorCode < 11000) {
        // Client errors
        logger.error(message);
    } else {
        // Other errors
        logger.error(message);
    }

    // Handle specific error codes
    if (errorCode == 502) {
        // Couldn't connect to TWS
        // This would typically be handled by the client's connect method
    } else if (errorCode == 504) {
        // Not connected
        // This would typically be handled by the client's connect method
    }
}

void IBWrapper::error(const std::string &str) {
    auto &logger = utils::Logger::get_instance();
    logger.error("IB API Error: " + str);
}

void IBWrapper::error(int id, int errorCode, const std::string &errorString) {
    error(id, errorCode, errorString, "");
}

// Market data methods
void IBWrapper::tickPrice(int tickerId, int field, double price,
                          [[maybe_unused]] const IB::TagValueList *attribs) {
    auto &logger = utils::Logger::get_instance();

    // Find the symbol for this ticker ID
    auto it = tickerIdToSymbol_.find(tickerId);
    if (it == tickerIdToSymbol_.end()) {
        logger.warning("Received tick price for unknown ticker ID: " +
                       std::to_string(tickerId));
        return;
    }

    const std::string &symbol = it->second;

    // Get the current timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm *tm_info = std::localtime(&time);

    std::stringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");
    std::string timestamp = ss.str();

    // Create market data
    MarketData data;
    data.symbol = symbol;
    data.timestamp = timestamp;

    // Set the appropriate field based on the tick type
    switch (field) {
    case 1: // BID
        data.bid = price;
        break;
    case 2: // ASK
        data.ask = price;
        break;
    case 4: // LAST
        data.price = price;
        break;
    case 6: // HIGH
        data.high = price;
        break;
    case 7: // LOW
        data.low = price;
        break;
    case 9: // CLOSE
        data.close = price;
        break;
    case 14: // OPEN
        data.open = price;
        break;
    default:
        // Ignore other tick types
        return;
    }

    // Notify the client of the market data update
    // This would typically be handled by the client's market data callback
    // client_.onMarketDataUpdate(data);
}

void IBWrapper::tickSize(int tickerId, int field, int size) {
    auto &logger = utils::Logger::get_instance();

    // Find the symbol for this ticker ID
    auto it = tickerIdToSymbol_.find(tickerId);
    if (it == tickerIdToSymbol_.end()) {
        logger.warning("Received tick size for unknown ticker ID: " +
                       std::to_string(tickerId));
        return;
    }

    const std::string &symbol = it->second;

    // Get the current timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm *tm_info = std::localtime(&time);

    std::stringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");
    std::string timestamp = ss.str();

    // Create market data
    MarketData data;
    data.symbol = symbol;
    data.timestamp = timestamp;

    // Set the appropriate field based on the tick type
    switch (field) {
    case 0: // BID_SIZE
        data.bid_size = size;
        break;
    case 3: // ASK_SIZE
        data.ask_size = size;
        break;
    case 5: // LAST_SIZE
        data.volume = size;
        break;
    case 8: // VOLUME
        data.volume = size;
        break;
    default:
        // Ignore other tick types
        return;
    }

    // Notify the client of the market data update
    // This would typically be handled by the client's market data callback
    // client_.onMarketDataUpdate(data);
}

// Helper methods for converting IB API types to Thales types
MarketData IBWrapper::convertToMarketData(int tickerId, double price, int size,
                                          const std::string &timestamp) {
    // Find the symbol for this ticker ID
    auto it = tickerIdToSymbol_.find(tickerId);
    if (it == tickerIdToSymbol_.end()) {
        // Return empty market data if the ticker ID is not found
        return MarketData();
    }

    const std::string &symbol = it->second;

    // Create market data
    MarketData data;
    data.symbol = symbol;
    data.price = price;
    data.volume = size;
    data.timestamp = timestamp;

    return data;
}

// Minimal implementation of required methods to avoid compilation errors
void IBWrapper::tickString([[maybe_unused]] int tickerId,
                           [[maybe_unused]] int tickType,
                           [[maybe_unused]] const std::string &value) {}
void IBWrapper::tickGeneric([[maybe_unused]] int tickerId,
                            [[maybe_unused]] int tickType,
                            [[maybe_unused]] double value) {}
void IBWrapper::tickEFP(
    [[maybe_unused]] int tickerId, [[maybe_unused]] int tickType,
    [[maybe_unused]] double basisPoints,
    [[maybe_unused]] const std::string &formattedBasisPoints,
    [[maybe_unused]] double totalDividends, [[maybe_unused]] int holdDays,
    [[maybe_unused]] const std::string &futureLastTradeDate,
    [[maybe_unused]] double dividendImpact,
    [[maybe_unused]] double dividendsToLastTradeDate) {}
void IBWrapper::tickOptionComputation(
    [[maybe_unused]] int tickerId, [[maybe_unused]] int field,
    [[maybe_unused]] double impliedVol, [[maybe_unused]] double delta,
    [[maybe_unused]] double optPrice, [[maybe_unused]] double pvDividend,
    [[maybe_unused]] double gamma, [[maybe_unused]] double vega,
    [[maybe_unused]] double theta, [[maybe_unused]] double undPrice) {}
void IBWrapper::tickSnapshotEnd([[maybe_unused]] int reqId) {}
void IBWrapper::marketDataType([[maybe_unused]] int reqId,
                               [[maybe_unused]] int marketDataType) {}
void IBWrapper::realtimeBar(
    [[maybe_unused]] int reqId, [[maybe_unused]] long time,
    [[maybe_unused]] double open, [[maybe_unused]] double high,
    [[maybe_unused]] double low, [[maybe_unused]] double close,
    [[maybe_unused]] long volume, [[maybe_unused]] double wap,
    [[maybe_unused]] int count) {}
void IBWrapper::historicalData(
    [[maybe_unused]] int reqId, [[maybe_unused]] const std::string &date,
    [[maybe_unused]] double open, [[maybe_unused]] double high,
    [[maybe_unused]] double low, [[maybe_unused]] double close,
    [[maybe_unused]] long volume, [[maybe_unused]] int barCount,
    [[maybe_unused]] double WAP, [[maybe_unused]] int hasGaps) {}
void IBWrapper::historicalDataEnd(
    [[maybe_unused]] int reqId,
    [[maybe_unused]] const std::string &startDateStr,
    [[maybe_unused]] const std::string &endDateStr) {}
void IBWrapper::orderStatus(
    [[maybe_unused]] int orderId, [[maybe_unused]] const std::string &status,
    [[maybe_unused]] double filled, [[maybe_unused]] double remaining,
    [[maybe_unused]] double avgFillPrice, [[maybe_unused]] int permId,
    [[maybe_unused]] int parentId, [[maybe_unused]] double lastFillPrice,
    [[maybe_unused]] int clientId, [[maybe_unused]] const std::string &whyHeld,
    [[maybe_unused]] double mktCapPrice) {}
void IBWrapper::openOrderEnd() {}
void IBWrapper::managedAccounts(const std::string &accountsList) {
    // Store the managed accounts in the client
    client_.set_managed_accounts(accountsList);
}
void IBWrapper::nextValidId(int orderId) {
    // Set the next valid order ID in the client
    client_.set_next_request_id(orderId);
}

// Placeholder implementations for other methods
// These would be implemented as needed for the specific requirements of the
// trading bot
void IBWrapper::historicalDataUpdate([[maybe_unused]] int reqId,
                                     [[maybe_unused]] const IB::Bar &bar) {}
void IBWrapper::openOrder([[maybe_unused]] int orderId,
                          [[maybe_unused]] const IBContract &contract,
                          [[maybe_unused]] const IBOrder &order,
                          [[maybe_unused]] const IBOrderState &orderState) {}
void IBWrapper::completedOrder(
    [[maybe_unused]] const IBContract &contract,
    [[maybe_unused]] const IBOrder &order,
    [[maybe_unused]] const IBOrderState &orderState) {}
void IBWrapper::execDetails([[maybe_unused]] int reqId,
                            [[maybe_unused]] const IBContract &contract,
                            [[maybe_unused]] const IBExecution &execution) {}
void IBWrapper::execDetailsEnd([[maybe_unused]] int reqId) {}
void IBWrapper::updateAccountValue(
    [[maybe_unused]] const std::string &key,
    [[maybe_unused]] const std::string &val,
    [[maybe_unused]] const std::string &currency,
    [[maybe_unused]] const std::string &accountName) {}
void IBWrapper::updatePortfolio(
    [[maybe_unused]] const IBContract &contract,
    [[maybe_unused]] double position, [[maybe_unused]] double marketPrice,
    [[maybe_unused]] double marketValue, [[maybe_unused]] double averageCost,
    [[maybe_unused]] double unrealizedPNL, [[maybe_unused]] double realizedPNL,
    [[maybe_unused]] const std::string &accountName) {}
void IBWrapper::updateAccountTime(
    [[maybe_unused]] const std::string &timeStamp) {}
void IBWrapper::accountDownloadEnd(
    [[maybe_unused]] const std::string &accountName) {}
void IBWrapper::position([[maybe_unused]] const std::string &account,
                         [[maybe_unused]] const IBContract &contract,
                         [[maybe_unused]] double pos,
                         [[maybe_unused]] double avgCost) {}
void IBWrapper::positionEnd() {}
void IBWrapper::accountSummary([[maybe_unused]] int reqId,
                               [[maybe_unused]] const std::string &account,
                               [[maybe_unused]] const std::string &tag,
                               [[maybe_unused]] const std::string &value,
                               [[maybe_unused]] const std::string &currency) {}
void IBWrapper::accountSummaryEnd([[maybe_unused]] int reqId) {}
void IBWrapper::contractDetails(
    [[maybe_unused]] int reqId,
    [[maybe_unused]] const IBContractDetails &contractDetails) {}
void IBWrapper::contractDetailsEnd([[maybe_unused]] int reqId) {}
void IBWrapper::bondContractDetails(
    [[maybe_unused]] int reqId,
    [[maybe_unused]] const IBContractDetails &contractDetails) {}

} // namespace data
} // namespace thales
