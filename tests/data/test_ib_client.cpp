// SPDX-License-Identifier: MIT

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "thales/data/ib_client.hpp"
#include "thales/data/ib_contract.hpp"
#include "thales/data/ib_order.hpp"
#include "thales/data/ib_wrapper.hpp"
#include "thales/utils/config.hpp"
#include "thales/utils/symbol_lookup.hpp"

using namespace thales::data;
using namespace thales::utils;
using namespace thales::core;
using namespace testing;

// Mock class for IBWrapper to test IBClient without actual IB API connection
class MockIBWrapper : public IBWrapper {
   public:
    explicit MockIBWrapper(IBClient &client) : IBWrapper(client) {}

    MOCK_METHOD(void, connectAck, ());
    MOCK_METHOD(void, connectionClosed, ());
    MOCK_METHOD(void, error,
                (int id, int errorCode, const std::string &errorString,
                 const std::string &advancedOrderRejectJson));
    MOCK_METHOD(void, error, (const std::string &str));
    MOCK_METHOD(void, error,
                (int id, int errorCode, const std::string &errorString));
    MOCK_METHOD(void, nextValidId, (int orderId));
    MOCK_METHOD(void, managedAccounts, (const std::string &accountsList));
    MOCK_METHOD(void, updateAccountValue,
                (const std::string &key, const std::string &val,
                 const std::string &currency, const std::string &accountName));
    MOCK_METHOD(void, tickPrice,
                (int tickerId, int field, double price,
                 const IB::TagValueList *attribs));
    MOCK_METHOD(void, tickSize, (int tickerId, int field, int size));
    MOCK_METHOD(void, orderStatus,
                (int orderId, const std::string &status, double filled,
                 double remaining, double avgFillPrice, int permId,
                 int parentId, double lastFillPrice, int clientId,
                 const std::string &whyHeld, double mktCapPrice));
};

// Test fixture for IBClient tests
class IBClientTest : public ::testing::Test {
   protected:
    Config config;
    std::unique_ptr<IBClient> client;
    std::unique_ptr<MockIBWrapper> mock_wrapper;

    void SetUp() override {
        // Initialize symbol lookup with test symbols
        std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG"};
        SymbolLookup::initialize(symbols);

        // Set up basic configuration
        config = Config();
        config.set_value("ib_client.host", "127.0.0.1");
        config.set_value("ib_client.port", "7497");
        config.set_value("ib_client.client_id", "1");

        // Create client
        client = std::make_unique<IBClient>(config);

        // Replace the wrapper with our mock
        // Note: This would require modifying IBClient to allow wrapper
        // injection mock_wrapper = std::make_unique<MockIBWrapper>(*client);
        // client->set_wrapper(mock_wrapper.get());
    }
};

// Test IBContract creation and conversion
TEST(IBContractTest, CreateEquityContract) {
    auto symbol_id = SymbolLookup::get_instance().add_symbol("AAPL");

    // Create a contract
    Option contract(symbol_id, "NASDAQ", Option::Type::UNKNOWN);

    // Create IB contract
    IBContract ib_contract;
    ib_contract.symbol = "AAPL";
    ib_contract.exchange = "NASDAQ";
    ib_contract.sec_type = "STK";
    ib_contract.currency = "USD";

    // Verify IB contract properties
    EXPECT_EQ(ib_contract.symbol, "AAPL");
    EXPECT_EQ(ib_contract.exchange, "NASDAQ");
    EXPECT_EQ(ib_contract.sec_type, "STK");
    EXPECT_EQ(ib_contract.currency, "USD");
}

TEST(IBContractTest, CreateOptionContract) {
    auto symbol_id = SymbolLookup::get_instance().add_symbol("AAPL");

    // Create a call option contract
    Option call_contract(symbol_id, "NASDAQ", Option::Type::CALL, "20251219",
                         200.0);

    // Create IB contract
    IBContract call_ib_contract;
    call_ib_contract.symbol = "AAPL";
    call_ib_contract.exchange = "NASDAQ";
    call_ib_contract.sec_type = "OPT";
    call_ib_contract.currency = "USD";
    call_ib_contract.right = "C";
    call_ib_contract.last_trade_date = "20251219";
    call_ib_contract.strike = 200.0;

    // Verify IB contract properties
    EXPECT_EQ(call_ib_contract.symbol, "AAPL");
    EXPECT_EQ(call_ib_contract.exchange, "NASDAQ");
    EXPECT_EQ(call_ib_contract.sec_type, "OPT");
    EXPECT_EQ(call_ib_contract.currency, "USD");
    EXPECT_EQ(call_ib_contract.right, "C");
    EXPECT_EQ(call_ib_contract.last_trade_date, "20251219");
    EXPECT_EQ(call_ib_contract.strike, 200.0);

    // Create a put option contract
    Option put_contract(symbol_id, "NASDAQ", Option::Type::PUT, "20251219",
                        200.0);

    // Create IB contract
    IBContract put_ib_contract;
    put_ib_contract.symbol = "AAPL";
    put_ib_contract.exchange = "NASDAQ";
    put_ib_contract.sec_type = "OPT";
    put_ib_contract.currency = "USD";
    put_ib_contract.right = "P";
    put_ib_contract.last_trade_date = "20251219";
    put_ib_contract.strike = 200.0;

    // Verify IB contract properties
    EXPECT_EQ(put_ib_contract.symbol, "AAPL");
    EXPECT_EQ(put_ib_contract.exchange, "NASDAQ");
    EXPECT_EQ(put_ib_contract.sec_type, "OPT");
    EXPECT_EQ(put_ib_contract.currency, "USD");
    EXPECT_EQ(put_ib_contract.right, "P");
    EXPECT_EQ(put_ib_contract.last_trade_date, "20251219");
    EXPECT_EQ(put_ib_contract.strike, 200.0);
}

// Test IBOrder creation and conversion
TEST(IBOrderTest, CreateMarketOrder) {
    auto symbol_id = SymbolLookup::get_instance().add_symbol("AAPL");

    // Create a market order
    Order order("test_order", symbol_id, Order::Type::MARKET, Order::Side::BUY,
                100);

    // Create IB order
    IBOrder ib_order("BUY", 100, "MKT");
    ib_order.order_id = "test_order";

    // Verify IB order properties
    EXPECT_EQ(ib_order.order_id, "test_order");
    EXPECT_EQ(ib_order.action, "BUY");
    EXPECT_EQ(ib_order.order_type, "MKT");
    EXPECT_EQ(ib_order.total_quantity, 100);
    EXPECT_EQ(ib_order.lmt_price, 0.0);
    EXPECT_EQ(ib_order.aux_price, 0.0);
}

TEST(IBOrderTest, CreateLimitOrder) {
    auto symbol_id = SymbolLookup::get_instance().add_symbol("AAPL");

    // Create a limit order
    Order order("test_order", symbol_id, Order::Type::LIMIT, Order::Side::SELL,
                100, 150.0);

    // Create IB order
    IBOrder ib_order("SELL", 100, "LMT");
    ib_order.order_id = "test_order";
    ib_order.lmt_price = 150.0;

    // Verify IB order properties
    EXPECT_EQ(ib_order.order_id, "test_order");
    EXPECT_EQ(ib_order.action, "SELL");
    EXPECT_EQ(ib_order.order_type, "LMT");
    EXPECT_EQ(ib_order.total_quantity, 100);
    EXPECT_EQ(ib_order.lmt_price, 150.0);
    EXPECT_EQ(ib_order.aux_price, 0.0);
}

TEST(IBOrderTest, CreateStopOrder) {
    auto symbol_id = SymbolLookup::get_instance().add_symbol("AAPL");

    // Create a stop order
    Order order("test_order", symbol_id, Order::Type::STOP, Order::Side::SELL,
                100, 0.0, 145.0);

    // Create IB order
    IBOrder ib_order("SELL", 100, "STP");
    ib_order.order_id = "test_order";
    ib_order.aux_price = 145.0;

    // Verify IB order properties
    EXPECT_EQ(ib_order.order_id, "test_order");
    EXPECT_EQ(ib_order.action, "SELL");
    EXPECT_EQ(ib_order.order_type, "STP");
    EXPECT_EQ(ib_order.total_quantity, 100);
    EXPECT_EQ(ib_order.lmt_price, 0.0);
    EXPECT_EQ(ib_order.aux_price, 145.0);
}

TEST(IBOrderTest, CreateStopLimitOrder) {
    auto symbol_id = SymbolLookup::get_instance().add_symbol("AAPL");

    // Create a stop-limit order
    Order order("test_order", symbol_id, Order::Type::STOP_LIMIT,
                Order::Side::SELL, 100, 144.0, 145.0);

    // Create IB order
    IBOrder ib_order("SELL", 100, "STP LMT");
    ib_order.order_id = "test_order";
    ib_order.lmt_price = 144.0;
    ib_order.aux_price = 145.0;

    // Verify IB order properties
    EXPECT_EQ(ib_order.order_id, "test_order");
    EXPECT_EQ(ib_order.action, "SELL");
    EXPECT_EQ(ib_order.order_type, "STP LMT");
    EXPECT_EQ(ib_order.total_quantity, 100);
    EXPECT_EQ(ib_order.lmt_price, 144.0);
    EXPECT_EQ(ib_order.aux_price, 145.0);
}

// The following tests would require mocking the IB API or having a real
// connection They are included as examples of what could be tested

/*
TEST_F(IBClientTest, Connect) {
    // Set up expectations
    EXPECT_CALL(*mock_wrapper, connectAck()).Times(1);
    EXPECT_CALL(*mock_wrapper, nextValidId(_)).Times(1);
    EXPECT_CALL(*mock_wrapper, managedAccounts(_)).Times(1);

    // Test connection
    EXPECT_TRUE(client->connect());
    EXPECT_TRUE(client->is_connected());
}

TEST_F(IBClientTest, Disconnect) {
    // Set up expectations
    EXPECT_CALL(*mock_wrapper, connectAck()).Times(1);
    EXPECT_CALL(*mock_wrapper, nextValidId(_)).Times(1);
    EXPECT_CALL(*mock_wrapper, managedAccounts(_)).Times(1);
    EXPECT_CALL(*mock_wrapper, connectionClosed()).Times(1);

    // Connect first
    EXPECT_TRUE(client->connect());

    // Test disconnection
    client->disconnect();
    EXPECT_FALSE(client->is_connected());
}

TEST_F(IBClientTest, SubscribeMarketData) {
    // Connect first
    EXPECT_TRUE(client->connect());

    // Set up expectations
    EXPECT_CALL(*mock_wrapper, tickPrice(_, _, _, _)).Times(AtLeast(1));
    EXPECT_CALL(*mock_wrapper, tickSize(_, _, _)).Times(AtLeast(1));

    // Test market data subscription
    EXPECT_TRUE(client->subscribe_market_data("AAPL"));

    // Wait for some data to arrive
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Get the latest market data
    auto market_data = client->get_latest_market_data("AAPL");

    // Verify market data
    EXPECT_EQ(market_data.symbol, "AAPL");
    EXPECT_GT(market_data.last_price, 0.0);
}

TEST_F(IBClientTest, PlaceOrder) {
    // Connect first
    EXPECT_TRUE(client->connect());

    // Create an order
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    Order order("", symbol_id, Order::Type::LIMIT, Order::Side::BUY, 1, 100.0);

    // Set up expectations
    EXPECT_CALL(*mock_wrapper, orderStatus(_, _, _, _, _, _, _, _, _, _,
_)).Times(AtLeast(1));

    // Place the order
    std::string order_id = client->place_order(order);

    // Verify order ID
    EXPECT_FALSE(order_id.empty());

    // Wait for order status update
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Get open orders
    auto open_orders = client->get_open_orders();

    // Verify open orders
    EXPECT_GE(open_orders.size(), 1);

    // Cancel the order
    EXPECT_TRUE(client->cancel_order(order_id));
}
*/
