// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

#include "thales/core/order.hpp"
#include "thales/utils/symbol_lookup.hpp"

using namespace thales::core;
using namespace thales::utils;

class OrderTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Initialize symbol lookup with test symbols
        std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG"};
        SymbolLookup::initialize(symbols);
    }
};

TEST_F(OrderTest, DefaultConstructor) {
    Order::OrderParams params;
    Order order(params);

    EXPECT_EQ(order.order_id, 0);
    EXPECT_EQ(order.symbol_id, SymbolLookup::INVALID_SYMBOL_ID);
    EXPECT_EQ(order.type, Order::Type::MARKET);
    EXPECT_EQ(order.side, Order::Side::BUY);
    EXPECT_EQ(order.quantity, 0);
    EXPECT_EQ(order.price.limit, 0.0);
    EXPECT_EQ(order.price.stop, 0.0);
    EXPECT_EQ(order.status, Order::Status::PENDING);
    EXPECT_EQ(order.filled_quantity, 0);
    EXPECT_EQ(order.price.average_fill, 0.0);
}

TEST_F(OrderTest, ParameterizedConstructor) {
    uint32_t order_id = 123;
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    Order::Type type = Order::Type::LIMIT;
    Order::Side side = Order::Side::SELL;
    double quantity = 100.0;
    double price = 150.0;
    double stop_price = 145.0;
    Order::Status status = Order::Status::PARTIALLY_FILLED;
    double filled_quantity = 50.0;
    double avg_fill_price = 149.5;
    std::string timestamp = "2025-04-06 15:30:00";

    Price::PriceParams price_params;
    price_params.limit_price = price;
    price_params.stop_price = stop_price;
    price_params.avg_fill_price = avg_fill_price;

    Order::OrderParams params;
    params.order_id = order_id;
    params.symbol_id = symbol_id;
    params.type = type;
    params.side = side;
    params.quantity = quantity;
    params.price = Price(price_params);
    params.status = status;
    params.filled_quantity = filled_quantity;
    params.timestamp = timestamp;

    Order order(params);

    EXPECT_EQ(order.order_id, order_id);
    EXPECT_EQ(order.symbol_id, symbol_id);
    EXPECT_EQ(order.type, type);
    EXPECT_EQ(order.side, side);
    EXPECT_EQ(order.quantity, static_cast<uint16_t>(quantity));
    EXPECT_EQ(order.price.limit, price);
    EXPECT_EQ(order.price.stop, stop_price);
    EXPECT_EQ(order.status, status);
    EXPECT_EQ(order.filled_quantity, static_cast<uint16_t>(filled_quantity));
    EXPECT_EQ(order.price.average_fill, avg_fill_price);
    EXPECT_EQ(order.timestamp, timestamp);
}

TEST_F(OrderTest, GetRemainingQuantity) {
    Order::OrderParams params;
    params.order_id = 1;
    params.symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    params.type = Order::Type::MARKET;
    params.side = Order::Side::BUY;
    params.quantity = 100.0;

    Order order(params);
    EXPECT_EQ(order.get_remaining_quantity(), 100.0);

    order.filled_quantity = 30;
    EXPECT_EQ(order.get_remaining_quantity(), 70.0);

    order.filled_quantity = 100;
    EXPECT_EQ(order.get_remaining_quantity(), 0.0);
}

TEST_F(OrderTest, IsActive) {
    Order::OrderParams params;
    Order order(params);

    order.status = Order::Status::PENDING;
    EXPECT_TRUE(order.is_active());

    order.status = Order::Status::PARTIALLY_FILLED;
    EXPECT_TRUE(order.is_active());

    order.status = Order::Status::FILLED;
    EXPECT_FALSE(order.is_active());

    order.status = Order::Status::CANCELED;
    EXPECT_FALSE(order.is_active());

    order.status = Order::Status::REJECTED;
    EXPECT_FALSE(order.is_active());
}

TEST_F(OrderTest, TypeToString) {
    Order::OrderParams params;
    Order order(params);

    order.type = Order::Type::MARKET;
    EXPECT_EQ(order.type_to_string(), "MARKET");

    order.type = Order::Type::LIMIT;
    EXPECT_EQ(order.type_to_string(), "LIMIT");

    order.type = Order::Type::STOP;
    EXPECT_EQ(order.type_to_string(), "STOP");

    order.type = Order::Type::STOP_LIMIT;
    EXPECT_EQ(order.type_to_string(), "STOP_LIMIT");
}

TEST_F(OrderTest, SideToString) {
    Order::OrderParams params;
    Order order(params);

    order.side = Order::Side::BUY;
    EXPECT_EQ(order.side_to_string(), "BUY");

    order.side = Order::Side::SELL;
    EXPECT_EQ(order.side_to_string(), "SELL");
}

TEST_F(OrderTest, StatusToString) {
    Order::OrderParams params;
    Order order(params);

    order.status = Order::Status::PENDING;
    EXPECT_EQ(order.status_to_string(), "PENDING");

    order.status = Order::Status::FILLED;
    EXPECT_EQ(order.status_to_string(), "FILLED");

    order.status = Order::Status::PARTIALLY_FILLED;
    EXPECT_EQ(order.status_to_string(), "PARTIALLY_FILLED");

    order.status = Order::Status::CANCELED;
    EXPECT_EQ(order.status_to_string(), "CANCELED");

    order.status = Order::Status::REJECTED;
    EXPECT_EQ(order.status_to_string(), "REJECTED");
}
