// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

#include "thales/core/portfolio.hpp"
#include "thales/utils/config.hpp"
#include "thales/utils/symbol_lookup.hpp"

using namespace thales::core;
using namespace thales::utils;

class PortfolioTest : public ::testing::Test {
   protected:
    Config config;

    void SetUp() override {
        // Initialize symbol lookup with test symbols
        std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG"};
        SymbolLookup::initialize(symbols);

        // Set up basic configuration
        config = Config();
    }
};

TEST_F(PortfolioTest, Construction) {
    Portfolio portfolio(config);

    // Verify initialization
    EXPECT_TRUE(portfolio.initialize());

    // Initial portfolio should be empty
    auto positions = portfolio.get_positions();
    EXPECT_EQ(positions.size(), 0);

    auto orders = portfolio.get_open_orders();
    EXPECT_EQ(orders.size(), 0);

    // Initial portfolio values should be zero
    EXPECT_EQ(portfolio.get_total_value(), 0.0);
    EXPECT_EQ(portfolio.get_total_unrealized_pnl(), 0.0);
    EXPECT_EQ(portfolio.get_total_realized_pnl(), 0.0);
}

TEST_F(PortfolioTest, AddPosition) {
    Portfolio portfolio(config);
    portfolio.initialize();

    // Create a test position
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    Position position(symbol_id, "NASDAQ", Option::Type::CALL, 100, 150.0,
                      160.0);

    // Add position to portfolio
    portfolio.add_position(position);

    // Verify position was added
    auto positions = portfolio.get_positions();
    EXPECT_EQ(positions.size(), 1);

    // Verify position details
    auto retrieved_position = portfolio.get_position("AAPL");
    EXPECT_EQ(retrieved_position.contract.symbol_id, symbol_id);
    EXPECT_EQ(retrieved_position.quantity, 100);
    EXPECT_EQ(retrieved_position.average_price, 150.0);
    EXPECT_EQ(retrieved_position.last_price, 160.0);

    // Verify portfolio values
    EXPECT_EQ(portfolio.get_total_value(), 16000.0);
    EXPECT_EQ(portfolio.get_total_unrealized_pnl(), position.unrealized_pnl);
}

TEST_F(PortfolioTest, UpdatePosition) {
    Portfolio portfolio(config);
    portfolio.initialize();

    // Create and add a test position
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    Position position(symbol_id, "NASDAQ", Option::Type::CALL, 100, 150.0,
                      160.0);
    portfolio.add_position(position);

    // Update position with new price
    portfolio.update_position("AAPL", 170.0);

    // Verify position was updated
    auto updated_position = portfolio.get_position("AAPL");
    EXPECT_EQ(updated_position.last_price, 170.0);
    EXPECT_EQ(updated_position.unrealized_pnl,
              updated_position.get_unrealized_pnl());
    EXPECT_EQ(updated_position.get_unrealized_pnl(),
              2000.0);  // (170 - 150) * 100
}

TEST_F(PortfolioTest, AddOrder) {
    Portfolio portfolio(config);
    portfolio.initialize();

    // Create a test order
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    Order order("order1", symbol_id, Order::Type::LIMIT, Order::Side::BUY, 100,
                150.0);

    // Add order to portfolio
    portfolio.add_order(order);

    // Verify order was added
    auto orders = portfolio.get_open_orders();
    EXPECT_EQ(orders.size(), 1);

    // Verify order details
    auto symbol_orders = portfolio.get_orders("AAPL");
    EXPECT_EQ(symbol_orders.size(), 1);
    EXPECT_EQ(symbol_orders[0].order_id, "order1");
    EXPECT_EQ(symbol_orders[0].symbol_id, symbol_id);
    EXPECT_EQ(symbol_orders[0].type, Order::Type::LIMIT);
    EXPECT_EQ(symbol_orders[0].side, Order::Side::BUY);
    EXPECT_EQ(symbol_orders[0].quantity, 100);
    EXPECT_EQ(symbol_orders[0].price, 150.0);
    EXPECT_EQ(symbol_orders[0].status, Order::Status::PENDING);
}

TEST_F(PortfolioTest, UpdateOrder) {
    Portfolio portfolio(config);
    portfolio.initialize();

    // Create and add a test order
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    Order order("order1", symbol_id, Order::Type::LIMIT, Order::Side::BUY, 100,
                150.0);
    portfolio.add_order(order);

    // Update order status to partially filled
    portfolio.update_order("order1", Order::Status::PARTIALLY_FILLED, 50,
                           149.0);

    // Verify order was updated
    auto orders = portfolio.get_open_orders();
    EXPECT_EQ(orders.size(), 1);
    EXPECT_EQ(orders[0].status, Order::Status::PARTIALLY_FILLED);
    EXPECT_EQ(orders[0].filled_quantity, 50);
    EXPECT_EQ(orders[0].average_fill_price, 149.0);

    // Update order status to filled
    portfolio.update_order("order1", Order::Status::FILLED, 50, 151.0);

    // Verify order was updated and is no longer open
    orders = portfolio.get_open_orders();
    EXPECT_EQ(orders.size(), 0);

    // But it should still be in the orders list for the symbol
    auto symbol_orders = portfolio.get_orders("AAPL");
    EXPECT_EQ(symbol_orders.size(), 1);
    EXPECT_EQ(symbol_orders[0].status, Order::Status::FILLED);
    EXPECT_EQ(symbol_orders[0].filled_quantity, 100);
    EXPECT_EQ(symbol_orders[0].average_fill_price,
              150.0);  // (149*50 + 151*50)/100
}

TEST_F(PortfolioTest, CancelOrder) {
    Portfolio portfolio(config);
    portfolio.initialize();

    // Create and add a test order
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    Order order("order1", symbol_id, Order::Type::LIMIT, Order::Side::BUY, 100,
                150.0);
    portfolio.add_order(order);

    // Cancel the order
    bool result = portfolio.cancel_order("order1");
    EXPECT_TRUE(result);

    // Verify order was canceled and is no longer open
    auto orders = portfolio.get_open_orders();
    EXPECT_EQ(orders.size(), 0);

    // But it should still be in the orders list for the symbol with CANCELED
    // status
    auto symbol_orders = portfolio.get_orders("AAPL");
    EXPECT_EQ(symbol_orders.size(), 1);
    EXPECT_EQ(symbol_orders[0].status, Order::Status::CANCELED);
}

TEST_F(PortfolioTest, MultiplePositionsAndOrders) {
    Portfolio portfolio(config);
    portfolio.initialize();

    // Add multiple positions
    auto aapl_id = SymbolLookup::get_instance().get_id("AAPL");
    auto msft_id = SymbolLookup::get_instance().get_id("MSFT");
    auto goog_id = SymbolLookup::get_instance().get_id("GOOG");

    Position aapl_position(aapl_id, "NASDAQ", Option::Type::CALL, 100, 150.0,
                           160.0);
    Position msft_position(msft_id, "NASDAQ", Option::Type::PUT, 50, 250.0,
                           240.0);
    Position goog_position(goog_id, "NASDAQ", Option::Type::CALL, 25, 2000.0,
                           2100.0);

    portfolio.add_position(aapl_position);
    portfolio.add_position(msft_position);
    portfolio.add_position(goog_position);

    // Add multiple orders
    Order aapl_order("order1", aapl_id, Order::Type::LIMIT, Order::Side::BUY,
                     50, 155.0);
    Order msft_order("order2", msft_id, Order::Type::MARKET, Order::Side::SELL,
                     25);
    Order goog_order("order3", goog_id, Order::Type::STOP, Order::Side::SELL,
                     10, 0.0, 2050.0);

    portfolio.add_order(aapl_order);
    portfolio.add_order(msft_order);
    portfolio.add_order(goog_order);

    // Verify positions
    auto positions = portfolio.get_positions();
    EXPECT_EQ(positions.size(), 3);

    // Verify orders
    auto orders = portfolio.get_open_orders();
    EXPECT_EQ(orders.size(), 3);

    // Verify portfolio values
    double expected_value = aapl_position.get_value() +
                            msft_position.get_value() +
                            goog_position.get_value();

    double expected_unrealized_pnl = aapl_position.unrealized_pnl +
                                     msft_position.unrealized_pnl +
                                     goog_position.unrealized_pnl;

    EXPECT_EQ(portfolio.get_total_value(), expected_value);
    EXPECT_EQ(portfolio.get_total_unrealized_pnl(), expected_unrealized_pnl);
}

TEST_F(PortfolioTest, PositionNotFound) {
    Portfolio portfolio(config);
    portfolio.initialize();

    // Try to get a position that doesn't exist
    auto position = portfolio.get_position("AAPL");

    // Should return an empty position
    EXPECT_EQ(position.contract.symbol_id, SymbolLookup::INVALID_SYMBOL_ID);
    EXPECT_EQ(position.quantity, 0);
}

TEST_F(PortfolioTest, OrderNotFound) {
    Portfolio portfolio(config);
    portfolio.initialize();

    // Try to cancel an order that doesn't exist
    bool result = portfolio.cancel_order("non_existent_order");
    EXPECT_FALSE(result);

    // Try to get orders for a symbol that has none
    auto orders = portfolio.get_orders("AAPL");
    EXPECT_EQ(orders.size(), 0);
}
