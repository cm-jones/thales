#include <gtest/gtest.h>
#include <thales/core/position.hpp>
#include <thales/utils/symbol_lookup.hpp>

using namespace thales::core;
using namespace thales::utils;

class PositionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize symbol lookup with test symbols
        std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG"};
        SymbolLookup::initialize(symbols);
    }
};

TEST_F(PositionTest, DefaultConstructor) {
    Position position;
    
    EXPECT_EQ(position.contract.symbol_id, SymbolLookup::INVALID_SYMBOL_ID);
    EXPECT_EQ(position.contract.exchange, "");
    EXPECT_EQ(position.contract.type, Contract::Type::UNKNOWN);
    EXPECT_EQ(position.quantity, 0);
    EXPECT_EQ(position.average_price, 0.0);
    EXPECT_EQ(position.last_price, 0.0);
    EXPECT_EQ(position.unrealized_pnl, 0.0);
    EXPECT_EQ(position.realized_pnl, 0.0);
}

TEST_F(PositionTest, ParameterizedConstructor) {
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    std::string exchange = "NASDAQ";
    Contract::Type option_type = Contract::Type::CALL;
    int quantity = 100;
    double avg_price = 150.0;
    double curr_price = 155.0;
    double unrealized_pnl = 500.0;
    double realized_pnl = 200.0;
    
    Position position(symbol_id, exchange, option_type, quantity, 
                     avg_price, curr_price, unrealized_pnl, realized_pnl);
    
    EXPECT_EQ(position.contract.symbol_id, symbol_id);
    EXPECT_EQ(position.contract.exchange, exchange);
    EXPECT_EQ(position.contract.type, option_type);
    EXPECT_EQ(position.quantity, static_cast<uint16_t>(quantity));
    EXPECT_EQ(position.average_price, avg_price);
    EXPECT_EQ(position.last_price, curr_price);
    EXPECT_EQ(position.unrealized_pnl, unrealized_pnl);
    EXPECT_EQ(position.realized_pnl, realized_pnl);
}

TEST_F(PositionTest, GetValue) {
    Position position;
    position.quantity = 100;
    position.last_price = 150.0;
    
    EXPECT_EQ(position.get_value(), 15000.0);
    
    position.quantity = 50;
    position.last_price = 200.0;
    
    EXPECT_EQ(position.get_value(), 10000.0);
    
    position.quantity = 0;
    EXPECT_EQ(position.get_value(), 0.0);
}

TEST_F(PositionTest, GetUnrealizedPnl) {
    Position position;
    position.quantity = 100;
    position.average_price = 150.0;
    position.last_price = 160.0;
    
    EXPECT_EQ(position.get_unrealized_pnl(), 1000.0);
    
    position.last_price = 140.0;
    EXPECT_EQ(position.get_unrealized_pnl(), -1000.0);
    
    position.quantity = 0;
    EXPECT_EQ(position.get_unrealized_pnl(), 0.0);
}

TEST_F(PositionTest, PositionWithDifferentOptionTypes) {
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    std::string exchange = "NASDAQ";
    
    // Test with CALL option
    Position call_position(symbol_id, exchange, Contract::Type::CALL, 100, 150.0, 160.0);
    EXPECT_EQ(call_position.contract.type, Contract::Type::CALL);
    EXPECT_EQ(call_position.get_unrealized_pnl(), 1000.0);
    
    // Test with PUT option
    Position put_position(symbol_id, exchange, Contract::Type::PUT, 100, 150.0, 140.0);
    EXPECT_EQ(put_position.contract.type, Contract::Type::PUT);
    EXPECT_EQ(put_position.get_unrealized_pnl(), -1000.0);
}

TEST_F(PositionTest, PositionWithDifferentQuantities) {
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    
    // Long position
    Position long_position(symbol_id, "NASDAQ", Contract::Type::CALL, 100, 150.0, 160.0);
    EXPECT_EQ(long_position.quantity, 100);
    EXPECT_EQ(long_position.get_value(), 16000.0);
    EXPECT_EQ(long_position.get_unrealized_pnl(), 1000.0);
    
    // Short position (negative quantity)
    Position short_position(symbol_id, "NASDAQ", Contract::Type::CALL, -50, 150.0, 160.0);
    EXPECT_EQ(short_position.quantity, static_cast<uint16_t>(-50) & 0xFFFF); // Unsigned conversion
    EXPECT_EQ(short_position.get_value(), short_position.quantity * 160.0);
    EXPECT_EQ(short_position.get_unrealized_pnl(), short_position.quantity * (160.0 - 150.0));
}
