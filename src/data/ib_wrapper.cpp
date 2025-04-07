#include <thales/data/ib_wrapper.hpp>
#include <thales/data/ib_client.hpp>
#include <thales/utils/logger.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace thales {
namespace data {

IBWrapper::IBWrapper(IBClient& client)
    : client_(client) {
    // Initialize any necessary state
}

IBWrapper::~IBWrapper() {
    // Clean up any resources
}

// Connection and server methods
void IBWrapper::connectAck() {
    auto& logger = utils::Logger::get_instance();
    logger.info("Connection to IB TWS/Gateway acknowledged");
}

void IBWrapper::connectionClosed() {
    auto& logger = utils::Logger::get_instance();
    logger.info("Connection to IB TWS/Gateway closed");
    
    // Notify the client that the connection was closed
    // This would typically be handled by the client's disconnect method
}

void IBWrapper::currentTime(long time) {
    auto& logger = utils::Logger::get_instance();
    
    // Convert time_t to string
    std::time_t t = static_cast<std::time_t>(time);
    std::tm* tm_info = std::localtime(&t);
    
    std::stringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");
    
    logger.debug("IB server time: " + ss.str());
}

void IBWrapper::error(int id, int errorCode, const std::string& errorString, const std::string& advancedOrderRejectJson) {
    auto& logger = utils::Logger::get_instance();
    
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

void IBWrapper::error(const std::string& str) {
    auto& logger = utils::Logger::get_instance();
    logger.error("IB API Error: " + str);
}

void IBWrapper::error(int id, int errorCode, const std::string& errorString) {
    error(id, errorCode, errorString, "");
}

// Market data methods
void IBWrapper::tickPrice(int tickerId, int field, double price, const IB::TagValueList* attribs) {
    auto& logger = utils::Logger::get_instance();
    
    // Find the symbol for this ticker ID
    auto it = tickerIdToSymbol_.find(tickerId);
    if (it == tickerIdToSymbol_.end()) {
        logger.warning("Received tick price for unknown ticker ID: " + std::to_string(tickerId));
        return;
    }
    
    const std::string& symbol = it->second;
    
    // Get the current timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&time);
    
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
    auto& logger = utils::Logger::get_instance();
    
    // Find the symbol for this ticker ID
    auto it = tickerIdToSymbol_.find(tickerId);
    if (it == tickerIdToSymbol_.end()) {
        logger.warning("Received tick size for unknown ticker ID: " + std::to_string(tickerId));
        return;
    }
    
    const std::string& symbol = it->second;
    
    // Get the current timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&time);
    
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
MarketData IBWrapper::convertToMarketData(int tickerId, double price, int size, const std::string& timestamp) {
    // Find the symbol for this ticker ID
    auto it = tickerIdToSymbol_.find(tickerId);
    if (it == tickerIdToSymbol_.end()) {
        // Return empty market data if the ticker ID is not found
        return MarketData();
    }
    
    const std::string& symbol = it->second;
    
    // Create market data
    MarketData data;
    data.symbol = symbol;
    data.price = price;
    data.volume = size;
    data.timestamp = timestamp;
    
    return data;
}

// Minimal implementation of required methods to avoid compilation errors
void IBWrapper::tickString(int tickerId, int tickType, const std::string& value) {}
void IBWrapper::tickGeneric(int tickerId, int tickType, double value) {}
void IBWrapper::tickEFP(int tickerId, int tickType, double basisPoints, const std::string& formattedBasisPoints,
                      double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact,
                      double dividendsToLastTradeDate) {}
void IBWrapper::tickOptionComputation(int tickerId, int field, double impliedVol, double delta,
                                    double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice) {}
void IBWrapper::tickSnapshotEnd(int reqId) {}
void IBWrapper::marketDataType(int reqId, int marketDataType) {}
void IBWrapper::realtimeBar(int reqId, long time, double open, double high, double low, double close,
                          long volume, double wap, int count) {}
void IBWrapper::historicalData(int reqId, const std::string& date, double open, double high,
                             double low, double close, long volume, int barCount, double WAP, int hasGaps) {}
void IBWrapper::historicalDataEnd(int reqId, const std::string& startDateStr, const std::string& endDateStr) {}
void IBWrapper::orderStatus(int orderId, const std::string& status, double filled,
                          double remaining, double avgFillPrice, int permId, int parentId,
                          double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice) {}
void IBWrapper::openOrderEnd() {}
void IBWrapper::managedAccounts(const std::string& accountsList) {
    // Store the managed accounts in the client
    client_.setManagedAccounts(accountsList);
}
void IBWrapper::nextValidId(int orderId) {
    // Set the next valid order ID in the client
    client_.setNextRequestId(orderId);
}

// Placeholder implementations for other methods
// These would be implemented as needed for the specific requirements of the trading bot
void IBWrapper::historicalDataUpdate(int reqId, const IB::Bar& bar) {}
void IBWrapper::openOrder(int orderId, const IBContract& contract, const IBOrder& order, const IBOrderState& orderState) {}
void IBWrapper::completedOrder(const IBContract& contract, const IBOrder& order, const IBOrderState& orderState) {}
void IBWrapper::execDetails(int reqId, const IBContract& contract, const IBExecution& execution) {}
void IBWrapper::execDetailsEnd(int reqId) {}
void IBWrapper::updateAccountValue(const std::string& key, const std::string& val,
                                 const std::string& currency, const std::string& accountName) {}
void IBWrapper::updatePortfolio(const IBContract& contract, double position,
                              double marketPrice, double marketValue, double averageCost,
                              double unrealizedPNL, double realizedPNL, const std::string& accountName) {}
void IBWrapper::updateAccountTime(const std::string& timeStamp) {}
void IBWrapper::accountDownloadEnd(const std::string& accountName) {}
void IBWrapper::position(const std::string& account, const IBContract& contract, double pos, double avgCost) {}
void IBWrapper::positionEnd() {}
void IBWrapper::accountSummary(int reqId, const std::string& account, const std::string& tag,
                             const std::string& value, const std::string& currency) {}
void IBWrapper::accountSummaryEnd(int reqId) {}
void IBWrapper::contractDetails(int reqId, const IBContractDetails& contractDetails) {}
void IBWrapper::contractDetailsEnd(int reqId) {}
void IBWrapper::bondContractDetails(int reqId, const IBContractDetails& contractDetails) {}

} // namespace data
} // namespace thales
