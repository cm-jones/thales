// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

#include "thales/core/position.hpp"
#include "thales/utils/symbol_lookup.hpp"

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

    EXPECT_EQ(position.option.symbol_id, SymbolLookup::INVALID_SYMBOL_ID);
    EXPECT_EQ(std::string(position.option.exchange.data()), "");
    EXPECT_EQ(position.option.type, Option::Type::UNKNOWN);
    EXPECT_EQ(position.quantity, 0);
    EXPECT_EQ(position.average_price, 0.0);
    EXPECT_EQ(position.last_price, 0.0);
    EXPECT_EQ(position.unrealized_pnl, 0.0);
    EXPECT_EQ(position.realized_pnl, 0.0);
}

TEST_F(PositionTest, ParameterizedConstructor) {
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    std::string exchange = "NASDAQ";
    Option::Type option_type = Option::Type::CALL;
    int quantity = 100;
    double avg_price = 150.0;
    double curr_price = 155.0;
    double unrealized_pnl = 500.0;
    double realized_pnl = 200.0;

    PositionParams params;
    params.id = symbol_id;
    params.exchange = exchange;
    params.option_type = option_type;
    params.quantity = quantity;
    params.avg_price = avg_price;
    params.curr_price = curr_price;
    params.unrealized_pnl = unrealized_pnl;
    params.realized_pnl = realized_pnl;

    Position position(params);

    EXPECT_EQ(position.option.symbol_id, symbol_id);
    EXPECT_EQ(std::string(position.option.exchange.data()), exchange);
    EXPECT_EQ(position.option.type, option_type);
    EXPECT_EQ(position.quantity, static_cast<uint16_t>(quantity));
    EXPECT_EQ(position.average_price, avg_price);
    EXPECT_EQ(position.last_price, curr_price);
    EXPECT_EQ(position.unrealized_pnl, unrealized_pnl);
    EXPECT_EQ(position.realized_pnl, realized_pnl);
}

TEST_F(PositionTest, GetValue) {
    PositionParams params;
    params.quantity = 100;
    params.curr_price = 150.0;
    Position position(params);

    EXPECT_EQ(position.get_value(), 15000.0);

    position.quantity = 50;
    position.last_price = 200.0;

    EXPECT_EQ(position.get_value(), 10000.0);

    position.quantity = 0;
    EXPECT_EQ(position.get_value(), 0.0);
}

TEST_F(PositionTest, GetUnrealizedPnl) {
    PositionParams params;
    params.quantity = 100;
    params.avg_price = 150.0;
    params.curr_price = 160.0;
    Position position(params);

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
    PositionParams call_params;
    call_params.id = symbol_id;
    call_params.exchange = exchange;
    call_params.option_type = Option::Type::CALL;
    call_params.quantity = 100;
    call_params.avg_price = 150.0;
    call_params.curr_price = 160.0;
    Position call_position(call_params);

    EXPECT_EQ(call_position.option.type, Option::Type::CALL);
    EXPECT_EQ(std::string(call_position.option.exchange.data()), exchange);
    EXPECT_EQ(call_position.get_unrealized_pnl(), 1000.0);

    // Test with PUT option
    PositionParams put_params;
    put_params.id = symbol_id;
    put_params.exchange = exchange;
    put_params.option_type = Option::Type::PUT;
    put_params.quantity = 100;
    put_params.avg_price = 150.0;
    put_params.curr_price = 140.0;
    Position put_position(put_params);

    EXPECT_EQ(put_position.option.type, Option::Type::PUT);
    EXPECT_EQ(std::string(put_position.option.exchange.data()), exchange);
    EXPECT_EQ(put_position.get_unrealized_pnl(), -1000.0);
}

TEST_F(PositionTest, PositionWithDifferentQuantities) {
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");

    // Long position
    PositionParams long_params;
    long_params.id = symbol_id;
    long_params.exchange = "NASDAQ";
    long_params.option_type = Option::Type::CALL;
    long_params.quantity = 100;
    long_params.avg_price = 150.0;
    long_params.curr_price = 160.0;
    Position long_position(long_params);

    EXPECT_EQ(long_position.quantity, 100);
    EXPECT_EQ(long_position.get_value(), 16000.0);
    EXPECT_EQ(long_position.get_unrealized_pnl(), 1000.0);

    // Short position (negative quantity)
    PositionParams short_params;
    short_params.id = symbol_id;
    short_params.exchange = "NASDAQ";
    short_params.option_type = Option::Type::CALL;
    short_params.quantity = -50;
    short_params.avg_price = 150.0;
    short_params.curr_price = 160.0;
    Position short_position(short_params);

    EXPECT_EQ(short_position.quantity,
              static_cast<uint16_t>(-50) & 0xFFFF);  // Unsigned conversion
    EXPECT_EQ(short_position.get_value(), short_position.quantity * 160.0);
    EXPECT_EQ(short_position.get_unrealized_pnl(),
              short_position.quantity * (160.0 - 150.0));
}
