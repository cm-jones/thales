#include <gtest/gtest.h>
#include <thales/core/order.hpp>
#include <thales/utils/symbol_lookup.hpp>

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
    Order order;
    
    EXPECT_EQ(order.order_id, "");
    EXPECT_EQ(order.symbol_id, SymbolLookup::INVALID_SYMBOL_ID);
    EXPECT_EQ(order.type, Order::Type::MARKET);
    EXPECT_EQ(order.side, Order::Side::BUY);
    EXPECT_EQ(order.quantity, 0);
    EXPECT_EQ(order.price, 0.0);
    EXPECT_EQ(order.stop_price, 0.0);
    EXPECT_EQ(order.status, Order::Status::PENDING);
    EXPECT_EQ(order.filled_quantity, 0);
    EXPECT_EQ(order.average_fill_price, 0.0);
}

TEST_F(OrderTest, ParameterizedConstructor) {
    std::string order_id = "test_order_123";
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
    
    Order order(order_id, symbol_id, type, side, quantity, price, stop_price, 
                status, filled_quantity, avg_fill_price, timestamp);
    
    EXPECT_EQ(order.order_id, order_id);
    EXPECT_EQ(order.symbol_id, symbol_id);
    EXPECT_EQ(order.type, type);
    EXPECT_EQ(order.side, side);
    EXPECT_EQ(order.quantity, static_cast<uint16_t>(quantity));
    EXPECT_EQ(order.price, price);
    EXPECT_EQ(order.stop_price, stop_price);
    EXPECT_EQ(order.status, status);
    EXPECT_EQ(order.filled_quantity, static_cast<uint16_t>(filled_quantity));
    EXPECT_EQ(order.average_fill_price, avg_fill_price);
    EXPECT_EQ(order.timestamp, timestamp);
}

TEST_F(OrderTest, GetRemainingQuantity) {
    Order order("test_order", 1, Order::Type::MARKET, Order::Side::BUY, 100.0);
    EXPECT_EQ(order.get_remaining_quantity(), 100.0);
    
    order.filled_quantity = 30;
    EXPECT_EQ(order.get_remaining_quantity(), 70.0);
    
    order.filled_quantity = 100;
    EXPECT_EQ(order.get_remaining_quantity(), 0.0);
}

TEST_F(OrderTest, IsActive) {
    Order order;
    
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
    Order order;
    
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
    Order order;
    
    order.side = Order::Side::BUY;
    EXPECT_EQ(order.side_to_string(), "BUY");
    
    order.side = Order::Side::SELL;
    EXPECT_EQ(order.side_to_string(), "SELL");
}

TEST_F(OrderTest, StatusToString) {
    Order order;
    
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
