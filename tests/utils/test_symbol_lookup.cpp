// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "thales/utils/symbol_lookup.hpp"

using namespace thales::utils;

class SymbolLookupTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Reset the singleton for each test by creating a new instance
        // with an empty vector (this is a bit of a hack but necessary for
        // testing)
        SymbolLookup::initialize({});
    }

    void TearDown() override {
        // Reset the singleton after each test
        SymbolLookup::initialize({});
    }
};

TEST_F(SymbolLookupTest, Initialization) {
    std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG", "AMZN"};
    SymbolLookup::initialize(symbols);

    SymbolLookup &lookup = SymbolLookup::get_instance();

    // Check size
    EXPECT_EQ(lookup.size(), symbols.size());

    // Check all symbols were added
    for (const auto &symbol : symbols) {
        EXPECT_TRUE(lookup.has_symbol(symbol));
        EXPECT_NE(lookup.get_id(symbol), SymbolLookup::INVALID_SYMBOL_ID);
    }

    // Check all IDs were assigned
    for (size_t i = 0; i < symbols.size(); ++i) {
        SymbolLookup::SymbolID id = i + 1;  // IDs start from 1
        EXPECT_TRUE(lookup.has_id(id));
        EXPECT_FALSE(lookup.get_symbol(id).empty());
    }
}

TEST_F(SymbolLookupTest, AddSymbol) {
    SymbolLookup &lookup = SymbolLookup::get_instance();

    // Add a symbol
    auto id1 = lookup.add_symbol("AAPL");
    EXPECT_NE(id1, SymbolLookup::INVALID_SYMBOL_ID);
    EXPECT_TRUE(lookup.has_symbol("AAPL"));
    EXPECT_TRUE(lookup.has_id(id1));
    EXPECT_EQ(lookup.get_symbol(id1), "AAPL");
    EXPECT_EQ(lookup.get_id("AAPL"), id1);

    // Add another symbol
    auto id2 = lookup.add_symbol("MSFT");
    EXPECT_NE(id2, SymbolLookup::INVALID_SYMBOL_ID);
    EXPECT_NE(id2, id1);
    EXPECT_TRUE(lookup.has_symbol("MSFT"));
    EXPECT_TRUE(lookup.has_id(id2));
    EXPECT_EQ(lookup.get_symbol(id2), "MSFT");
    EXPECT_EQ(lookup.get_id("MSFT"), id2);

    // Add the same symbol again (should return the same ID)
    auto id3 = lookup.add_symbol("AAPL");
    EXPECT_EQ(id3, id1);
    EXPECT_EQ(lookup.size(), 2);  // Size should still be 2
}

TEST_F(SymbolLookupTest, GetId) {
    SymbolLookup &lookup = SymbolLookup::get_instance();

    // Add some symbols
    auto id1 = lookup.add_symbol("AAPL");
    auto id2 = lookup.add_symbol("MSFT");
    auto id3 = lookup.add_symbol("GOOG");

    // Get IDs for existing symbols
    EXPECT_EQ(lookup.get_id("AAPL"), id1);
    EXPECT_EQ(lookup.get_id("MSFT"), id2);
    EXPECT_EQ(lookup.get_id("GOOG"), id3);

    // Get ID for non-existent symbol
    EXPECT_EQ(lookup.get_id("INVALID"), SymbolLookup::INVALID_SYMBOL_ID);
}

TEST_F(SymbolLookupTest, GetSymbol) {
    SymbolLookup &lookup = SymbolLookup::get_instance();

    // Add some symbols
    auto id1 = lookup.add_symbol("AAPL");
    auto id2 = lookup.add_symbol("MSFT");
    auto id3 = lookup.add_symbol("GOOG");

    // Get symbols for existing IDs
    EXPECT_EQ(lookup.get_symbol(id1), "AAPL");
    EXPECT_EQ(lookup.get_symbol(id2), "MSFT");
    EXPECT_EQ(lookup.get_symbol(id3), "GOOG");

    // Get symbol for non-existent ID
    EXPECT_EQ(lookup.get_symbol(999), "");
}

TEST_F(SymbolLookupTest, HasSymbol) {
    SymbolLookup &lookup = SymbolLookup::get_instance();

    // Add some symbols
    lookup.add_symbol("AAPL");
    lookup.add_symbol("MSFT");

    // Check existing symbols
    EXPECT_TRUE(lookup.has_symbol("AAPL"));
    EXPECT_TRUE(lookup.has_symbol("MSFT"));

    // Check non-existent symbol
    EXPECT_FALSE(lookup.has_symbol("GOOG"));
}

TEST_F(SymbolLookupTest, HasId) {
    SymbolLookup &lookup = SymbolLookup::get_instance();

    // Add some symbols
    auto id1 = lookup.add_symbol("AAPL");
    auto id2 = lookup.add_symbol("MSFT");

    // Check existing IDs
    EXPECT_TRUE(lookup.has_id(id1));
    EXPECT_TRUE(lookup.has_id(id2));

    // Check non-existent ID
    EXPECT_FALSE(lookup.has_id(999));
}

TEST_F(SymbolLookupTest, Size) {
    SymbolLookup &lookup = SymbolLookup::get_instance();

    // Initial size should be 0
    EXPECT_EQ(lookup.size(), 0);

    // Add some symbols
    lookup.add_symbol("AAPL");
    EXPECT_EQ(lookup.size(), 1);

    lookup.add_symbol("MSFT");
    EXPECT_EQ(lookup.size(), 2);

    lookup.add_symbol("GOOG");
    EXPECT_EQ(lookup.size(), 3);

    // Adding the same symbol again should not increase size
    lookup.add_symbol("AAPL");
    EXPECT_EQ(lookup.size(), 3);
}

TEST_F(SymbolLookupTest, GetAllSymbols) {
    SymbolLookup &lookup = SymbolLookup::get_instance();

    // Add some symbols
    std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG", "AMZN"};
    for (const auto &symbol : symbols) {
        lookup.add_symbol(symbol);
    }

    // Get all symbols
    auto all_symbols = lookup.get_all_symbols();

    // Check size
    EXPECT_EQ(all_symbols.size(), symbols.size());

    // Check all symbols are present
    for (const auto &symbol : symbols) {
        EXPECT_NE(std::find(all_symbols.begin(), all_symbols.end(), symbol),
                  all_symbols.end());
    }
}

TEST_F(SymbolLookupTest, Singleton) {
    // Initialize with some symbols
    std::vector<std::string> symbols = {"AAPL", "MSFT", "GOOG"};
    SymbolLookup::initialize(symbols);

    // Get the instance
    SymbolLookup &instance1 = SymbolLookup::get_instance();

    // Add a symbol
    auto id = instance1.add_symbol("AMZN");

    // Get the instance again
    SymbolLookup &instance2 = SymbolLookup::get_instance();

    // Check that it's the same instance
    EXPECT_EQ(instance2.get_id("AMZN"), id);
    EXPECT_EQ(instance2.size(), 4);
}
