#include <gtest/gtest.h>
#include <thales/core/contract.hpp>
#include <thales/utils/symbol_lookup.hpp>

using namespace thales::core;
using namespace thales::utils;

class ContractTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize symbol lookup with test symbols
        std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG"};
        SymbolLookup::initialize(symbols);
    }
};

TEST_F(ContractTest, DefaultConstructor) {
    Option contract;
    
    EXPECT_EQ(contract.symbol_id, SymbolLookup::INVALID_SYMBOL_ID);
    EXPECT_EQ(contract.exchange, "");
    EXPECT_EQ(contract.type, Option::Type::UNKNOWN);
    EXPECT_EQ(contract.expiry, "");
    EXPECT_EQ(contract.strike, 0.0);
    
    // Check default Greeks values
    EXPECT_EQ(contract.greeks.delta, 0.0);
    EXPECT_EQ(contract.greeks.gamma, 0.0);
    EXPECT_EQ(contract.greeks.theta, 0.0);
    EXPECT_EQ(contract.greeks.vega, 0.0);
    EXPECT_EQ(contract.greeks.rho, 0.0);
}

TEST_F(ContractTest, ParameterizedConstructor) {
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    std::string exchange = "NASDAQ";
    Option::Type type = Option::Type::CALL;
    std::string expiry = "2025-12-19";
    double strike = 200.0;
    
    Option contract(symbol_id, exchange, type, expiry, strike);
    
    EXPECT_EQ(contract.symbol_id, symbol_id);
    EXPECT_EQ(contract.exchange, exchange);
    EXPECT_EQ(contract.type, type);
    EXPECT_EQ(contract.expiry, expiry);
    EXPECT_EQ(contract.strike, strike);
}

TEST_F(ContractTest, GreeksInitialization) {
    Option contract;
    
    // Initialize Greeks
    contract.greeks.delta = 0.65;
    contract.greeks.gamma = 0.05;
    contract.greeks.theta = -0.15;
    contract.greeks.vega = 0.25;
    contract.greeks.rho = 0.10;
    
    EXPECT_EQ(contract.greeks.delta, 0.65);
    EXPECT_EQ(contract.greeks.gamma, 0.05);
    EXPECT_EQ(contract.greeks.theta, -0.15);
    EXPECT_EQ(contract.greeks.vega, 0.25);
    EXPECT_EQ(contract.greeks.rho, 0.10);
}

TEST_F(ContractTest, ContractWithDifferentOptionTypes) {
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    std::string exchange = "NASDAQ";
    std::string expiry = "2025-12-19";
    double strike = 200.0;
    
    // Test with CALL option
    Option call_contract(symbol_id, exchange, Option::Type::CALL, expiry, strike);
    EXPECT_EQ(call_contract.type, Option::Type::CALL);
    
    // Test with PUT option
    Option put_contract(symbol_id, exchange, Option::Type::PUT, expiry, strike);
    EXPECT_EQ(put_contract.type, Option::Type::PUT);
}

TEST_F(ContractTest, ContractWithDifferentSymbols) {
    std::string exchange = "NASDAQ";
    Option::Type type = Option::Type::CALL;
    std::string expiry = "2025-12-19";
    double strike = 200.0;
    
    // Test with different symbols
    auto aapl_id = SymbolLookup::get_instance().get_id("AAPL");
    Option aapl_contract(aapl_id, exchange, type, expiry, strike);
    EXPECT_EQ(aapl_contract.symbol_id, aapl_id);
    
    auto msft_id = SymbolLookup::get_instance().get_id("MSFT");
    Option msft_contract(msft_id, exchange, type, expiry, strike);
    EXPECT_EQ(msft_contract.symbol_id, msft_id);
    
    auto goog_id = SymbolLookup::get_instance().get_id("GOOG");
    Option goog_contract(goog_id, exchange, type, expiry, strike);
    EXPECT_EQ(goog_contract.symbol_id, goog_id);
}

TEST_F(ContractTest, ContractWithDifferentExpiryAndStrike) {
    auto symbol_id = SymbolLookup::get_instance().get_id("AAPL");
    std::string exchange = "NASDAQ";
    Option::Type type = Option::Type::CALL;
    
    // Test with different expiry dates and strikes
    Option contract1(symbol_id, exchange, type, "2025-06-20", 180.0);
    EXPECT_EQ(contract1.expiry, "2025-06-20");
    EXPECT_EQ(contract1.strike, 180.0);
    
    Option contract2(symbol_id, exchange, type, "2025-09-19", 190.0);
    EXPECT_EQ(contract2.expiry, "2025-09-19");
    EXPECT_EQ(contract2.strike, 190.0);
    
    Option contract3(symbol_id, exchange, type, "2025-12-19", 200.0);
    EXPECT_EQ(contract3.expiry, "2025-12-19");
    EXPECT_EQ(contract3.strike, 200.0);
}
