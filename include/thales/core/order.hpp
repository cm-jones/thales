// SPDX-License-Identifier: MIT

#pragma once

#include <string>

#include "thales/core/option.hpp"
#include "thales/utils/symbol_lookup.hpp"

namespace thales {
namespace core {

/// Price information for an order
///
/// Encapsulates different types of prices associated with an order:
/// - Limit price for limit orders
/// - Stop price for stop/stop-limit orders
/// - Average fill price for executed orders
///
/// Uses a parameter struct pattern to avoid confusion when constructing prices.
struct CACHE_ALIGNED Price {
    double limit;         // Limit price for the order
    double stop;          // Stop price for stop/stop-limit orders
    double average_fill;  // Average fill price for executed orders

    /// Default constructor initializes all prices to zero
    Price() : limit(0.0), stop(0.0), average_fill(0.0) {}

    /// Parameters for constructing a Price object
    struct PriceParams {
        double limit_price = 0.0;
        double stop_price = 0.0;
        double avg_fill_price = 0.0;
    };

    /// Construct a Price with specific parameters
    explicit Price(const PriceParams &params)
        : limit(params.limit_price),
          stop(params.stop_price),
          average_fill(params.avg_fill_price) {}

    /// Factory Methods

    /// Create a Price object for market orders (all prices zero)
    static Price market_price() { return Price(); }

    /// Create a Price object for limit orders
    /// @param limit_value The limit price for the order
    static Price limit_price(double limit_value) {
        PriceParams params;
        params.limit_price = limit_value;
        return Price(params);
    }

    /// Create a Price object for stop orders
    /// @param stop_value The stop price for the order
    static Price stop_price(double stop_value) {
        PriceParams params;
        params.stop_price = stop_value;
        return Price(params);
    }
};

/// Trading order to buy or sell a financial instrument
///
/// Represents a complete order with:
/// - Order identification (ID, timestamp)
/// - Trading details (symbol, quantity, side)
/// - Price information (limit, stop, average fill)
/// - Execution status (pending, filled, etc.)
///
/// Uses a parameter struct pattern for clear construction.
/// Includes utilities for order status checking and string conversion.
struct CACHE_ALIGNED Order {
    /// @enum Type
    /// @brief Defines the type of the order: MARKET, LIMIT, STOP, or
    /// STOP_LIMIT.
    enum struct Type : uint8_t {
        MARKET,
        LIMIT,
        STOP,
        STOP_LIMIT,
    };

    /// @enum Side
    /// @brief Defines the side of the order: BUY or SELL
    enum struct Side : uint8_t {
        BUY,
        SELL,
    };

    /// @enum Status
    /// @brief Defines the status of the order: PENDING, FILLED,
    /// PARTIALLY_FILLED, CANCELED, or REJECTED.
    enum struct Status : uint8_t {
        PENDING,
        FILLED,
        PARTIALLY_FILLED,
        CANCELED,
        REJECTED,
    };

    std::string timestamp;                    ///< Order creation timestamp
    Price price;                              ///< Order price information
    uint32_t order_id;                        ///< Unique order identifier
    uint16_t quantity;                        ///< Total quantity to trade
    uint16_t filled_quantity;                 ///< Amount already filled
    utils::SymbolLookup::SymbolID symbol_id;  ///< Trading instrument identifier
    Type type;      ///< Order type (MARKET, LIMIT, etc.)
    Side side;      ///< Trading side (BUY or SELL)
    Status status;  ///< Current order status

    /// Parameters for constructing an Order object
    struct OrderParams {
        uint32_t order_id = 0;
        utils::SymbolLookup::SymbolID symbol_id =
            utils::SymbolLookup::INVALID_SYMBOL_ID;
        Type type = Type::MARKET;
        Side side = Side::BUY;
        double quantity = 0.0;
        Price price = Price();
        Status status = Status::PENDING;
        double filled_quantity = 0.0;
        std::string timestamp = "";
    };

    /// Construct an Order with specific parameters
    explicit Order(const OrderParams &params);

    /// Calculate unfilled order quantity
    /// @return Remaining quantity to be filled
    double get_remaining_quantity() const;

    /// Check if order is still active
    /// @return true if order is pending or partially filled
    bool is_active() const;

    /// Get string representation of order type
    /// @return Order type as string
    std::string type_to_string() const;

    /// Get string representation of order side
    /// @return Order side as string
    std::string side_to_string() const;

    /// Get string representation of order status
    /// @return Order status as string
    std::string status_to_string() const;
};

}  // namespace core
}  // namespace thales
