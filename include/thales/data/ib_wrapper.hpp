#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <set>
#include <map>
#include <thales/data/market_data.hpp>
#include <thales/core/order.hpp>
#include <thales/core/position.hpp>
#include <thales/data/ib_contract.hpp>
#include <thales/data/ib_order.hpp>

// Forward declarations for IB API classes we don't need to fully implement
namespace IB {
class EWrapper;
class TagValueList;
class Bar;

// Simple struct for NewsProvider
struct NewsProvider {
    std::string code;
    std::string name;
};

// Simple struct for HistoricalTick
struct HistoricalTick {
    long time;
    double price;
    long size;
};

// Simple struct for HistoricalTickBidAsk
struct HistoricalTickBidAsk {
    long time;
    double bidPrice;
    double askPrice;
    long bidSize;
    long askSize;
};

// Simple struct for HistoricalTickLast
struct HistoricalTickLast {
    long time;
    double price;
    long size;
    std::string exchange;
    std::string specialConditions;
};

// Simple struct for TickAttribLast
struct TickAttribLast {
    bool pastLimit;
    bool unreported;
};

// Simple struct for TickAttribBidAsk
struct TickAttribBidAsk {
    bool bidPastLow;
    bool askPastHigh;
};

// Simple struct for SmartComponent
struct SmartComponent {
    int bitNumber;
    std::string exchange;
    char exchangeLetter;
};

// Simple struct for DepthMktDataDescription
struct DepthMktDataDescription {
    std::string exchange;
    std::string secType;
    std::string listingExch;
    std::string serviceDataType;
    int aggGroup;
};

// Simple struct for PriceIncrement
struct PriceIncrement {
    double lowEdge;
    double increment;
};

// Simple struct for HistogramEntry
struct HistogramEntry {
    double price;
    long size;
};

// Simple struct for FamilyCode
struct FamilyCode {
    std::string accountID;
    std::string familyCodeStr;
};

// Simple struct for SoftDollarTier
struct SoftDollarTier {
    std::string name;
    std::string value;
    std::string displayName;
};

// Simple struct for CommissionReport
struct CommissionReport {
    std::string execId;
    double commission;
    std::string currency;
    double realizedPNL;
    double yield;
    int yieldRedemptionDate;
};
}

namespace thales {
namespace data {

// Forward declaration
class IBClient;

/**
 * @class IBWrapper
 * @brief Wrapper class for handling callbacks from the Interactive Brokers API.
 * 
 * This class implements the IB::EWrapper interface and forwards the callbacks
 * to the IBClient class. It handles all the callbacks from the IB API and
 * converts them to the appropriate format for the IBClient class.
 */
class IBWrapper {
public:
    /**
     * @brief Constructor
     * @param client Reference to the IBClient instance
     */
    explicit IBWrapper(IBClient& client);

    /**
     * @brief Destructor
     */
    ~IBWrapper();

    // Connection and server methods
    void connectAck();
    void connectionClosed();
    void currentTime(long time);
    void error(int id, int errorCode, const std::string& errorString, const std::string& advancedOrderRejectJson);
    void error(const std::string& str);
    void error(int id, int errorCode, const std::string& errorString);

    // Market data methods
    void tickPrice(int tickerId, int field, double price, const IB::TagValueList* attribs);
    void tickSize(int tickerId, int field, int size);
    void tickString(int tickerId, int tickType, const std::string& value);
    void tickGeneric(int tickerId, int tickType, double value);
    void tickEFP(int tickerId, int tickType, double basisPoints, const std::string& formattedBasisPoints,
                double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact,
                double dividendsToLastTradeDate);
    void tickOptionComputation(int tickerId, int field, double impliedVol, double delta,
                              double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice);
    void tickSnapshotEnd(int reqId);
    void marketDataType(int reqId, int marketDataType);
    void realtimeBar(int reqId, long time, double open, double high, double low, double close,
                    long volume, double wap, int count);
    void historicalData(int reqId, const std::string& date, double open, double high,
                       double low, double close, long volume, int barCount, double WAP, int hasGaps);
    void historicalDataUpdate(int reqId, const IB::Bar& bar);
    void historicalDataEnd(int reqId, const std::string& startDateStr, const std::string& endDateStr);

    // Order status methods
    void orderStatus(int orderId, const std::string& status, double filled,
                    double remaining, double avgFillPrice, int permId, int parentId,
                    double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice);
    void openOrder(int orderId, const IBContract& contract, const IBOrder& order, const IBOrderState& orderState);
    void openOrderEnd();
    void completedOrder(const IBContract& contract, const IBOrder& order, const IBOrderState& orderState);
    void completedOrdersEnd();
    void execDetails(int reqId, const IBContract& contract, const IBExecution& execution);
    void execDetailsEnd(int reqId);
    void commissionReport(const IB::CommissionReport& commissionReport);

    // Account and portfolio methods
    void updateAccountValue(const std::string& key, const std::string& val,
                           const std::string& currency, const std::string& accountName);
    void updatePortfolio(const IBContract& contract, double position,
                        double marketPrice, double marketValue, double averageCost,
                        double unrealizedPNL, double realizedPNL, const std::string& accountName);
    void updateAccountTime(const std::string& timeStamp);
    void accountDownloadEnd(const std::string& accountName);
    void position(const std::string& account, const IBContract& contract, double pos, double avgCost);
    void positionEnd();
    void accountSummary(int reqId, const std::string& account, const std::string& tag,
                       const std::string& value, const std::string& currency);
    void accountSummaryEnd(int reqId);

    // Contract details methods
    void contractDetails(int reqId, const IBContractDetails& contractDetails);
    void contractDetailsEnd(int reqId);
    void bondContractDetails(int reqId, const IBContractDetails& contractDetails);

    // News methods
    void tickNews(int tickerId, long timeStamp, const std::string& providerCode, const std::string& articleId,
                 const std::string& headline, const std::string& extraData);
    void newsProviders(const std::vector<IB::NewsProvider>& newsProviders);
    void newsArticle(int requestId, int articleType, const std::string& articleText);
    void historicalNews(int requestId, const std::string& time, const std::string& providerCode,
                       const std::string& articleId, const std::string& headline);
    void historicalNewsEnd(int requestId, bool hasMore);

    // Other methods
    void managedAccounts(const std::string& accountsList);
    void receiveFA(int faDataType, const std::string& faXmlData);
    void scannerParameters(const std::string& xml);
    void scannerData(int reqId, int rank, const IBContractDetails& contractDetails,
                    const std::string& distance, const std::string& benchmark, const std::string& projection,
                    const std::string& legsStr);
    void scannerDataEnd(int reqId);
    void verifyMessageAPI(const std::string& apiData);
    void verifyCompleted(bool isSuccessful, const std::string& errorText);
    void verifyAndAuthMessageAPI(const std::string& apiData, const std::string& xyzChallenge);
    void verifyAndAuthCompleted(bool isSuccessful, const std::string& errorText);
    void displayGroupList(int reqId, const std::string& groups);
    void displayGroupUpdated(int reqId, const std::string& contractInfo);
    void securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId, const std::string& tradingClass,
                                           const std::string& multiplier, const std::set<std::string>& expirations, const std::set<double>& strikes);
    void securityDefinitionOptionalParameterEnd(int reqId);
    void softDollarTiers(int reqId, const std::vector<IB::SoftDollarTier>& tiers);
    void familyCodes(const std::vector<IB::FamilyCode>& familyCodes);
    // TODO: Define IBContractDescription class
    void symbolSamples(int reqId, const std::vector<IBContractDescription>& contractDescriptions);
    void mktDepthExchanges(const std::vector<IB::DepthMktDataDescription>& depthMktDataDescriptions);
    void smartComponents(int reqId, const std::map<int, IB::SmartComponent>& theMap);
    void tickReqParams(int tickerId, double minTick, const std::string& bboExchange, int snapshotPermissions);
    void headTimestamp(int reqId, const std::string& headTimestamp);
    void histogramData(int reqId, const std::vector<IB::HistogramEntry>& items);
    void rerouteMktDataReq(int reqId, int conId, const std::string& exchange);
    void rerouteMktDepthReq(int reqId, int conId, const std::string& exchange);
    void marketRule(int marketRuleId, const std::vector<IB::PriceIncrement>& priceIncrements);
    void pnl(int reqId, double dailyPnL, double unrealizedPnL, double realizedPnL);
    void pnlSingle(int reqId, int pos, double dailyPnL, double unrealizedPnL, double realizedPnL, double value);
    void historicalTicks(int reqId, const std::vector<IB::HistoricalTick>& ticks, bool done);
    void historicalTicksBidAsk(int reqId, const std::vector<IB::HistoricalTickBidAsk>& ticks, bool done);
    void historicalTicksLast(int reqId, const std::vector<IB::HistoricalTickLast>& ticks, bool done);
    void tickByTickAllLast(int reqId, int tickType, long time, double price, int size, const IB::TickAttribLast& tickAttribLast, const std::string& exchange, const std::string& specialConditions);
    void tickByTickBidAsk(int reqId, long time, double bidPrice, double askPrice, int bidSize, int askSize, const IB::TickAttribBidAsk& tickAttribBidAsk);
    void tickByTickMidPoint(int reqId, long time, double midPoint);
    void orderBound(long orderId, int apiClientId, int apiOrderId);
    void nextValidId(int orderId);

private:
    // Reference to the IBClient instance
    IBClient& client_;

    // Helper methods for converting IB API types to Thales types
    MarketData convertToMarketData(int tickerId, double price, int size, const std::string& timestamp);
    core::Order convertToOrder(const IBOrder& ibOrder, const IBContract& ibContract);
    core::Position convertToPosition(const IBContract& ibContract, double position, double marketPrice, double averageCost);

    // Maps for tracking requests
    std::unordered_map<int, std::string> tickerIdToSymbol_;
    std::unordered_map<int, std::string> orderIdToOrderId_;
};

} // namespace data
} // namespace thales
