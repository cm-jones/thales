#pragma once

#include <string>
#include <thales/utils/symbol_lookup.hpp>
#include "thales/core/option.hpp"

namespace thales {
namespace core {

/**
 * @struct Price
 * @brief Represents price information for an order to avoid parameter confusion.
 */
struct alignas(CACHE_LINE_SIZE) Price {
    double limit;            // Limit price for the order
    double stop;             // Stop price for stop/stop-limit orders
    double average_fill;     // Average fill price for executed orders
    
    // Default constructor
    Price() : limit(0.0), stop(0.0), average_fill(0.0) {}
    
    // Parameter struct for Price constructor to avoid parameter confusion
    struct PriceParams {
        double limit_price = 0.0;
        double stop_price = 0.0;
        double avg_fill_price = 0.0;
    };
    
    // Constructor with named parameter struct
    explicit Price(const PriceParams& params)
        : limit(params.limit_price), stop(params.stop_price), average_fill(params.avg_fill_price) {}
        
    // Factory methods to create Price objects with specific configurations
    static Price market_price() { return Price(); }
    
    static Price limit_price(double limit_value) {
        PriceParams params;
        params.limit_price = limit_value;
        return Price(params);
    }
    
    static Price stop_price(double stop_value) {
        PriceParams params;
        params.stop_price = stop_value;
        return Price(params);
    }
};

/**
 * @struct Order
 * @brief Represents an order to buy or sell a financial instrument.
 */
struct alignas(CACHE_LINE_SIZE) Order {
    /**
     * @enum Type
     * @brief Defines the type of the order: MARKET, LIMIT, STOP, or
     * STOP_LIMIT.
     */
    enum struct Type : uint8_t {
        MARKET,
        LIMIT,
        STOP,
        STOP_LIMIT,
    };

    /**
     * @enum Side
     * @brief Defines the side of the order: BUY or SELL
     */
    enum struct Side : uint8_t {
        BUY,
        SELL,
    };

    /**
     * @enum Status
     * @brief Defines the status of the order: PENDING, FILLED,
     * PARTIALLY_FILLED, CANCELED, or REJECTED.
     */
    enum struct Status : uint8_t {
        PENDING,
        FILLED,
        PARTIALLY_FILLED,
        CANCELED,
        REJECTED,
    };

    std::string timestamp;      // When the order was created
    Price price;                // Price information for the order
    uint32_t order_id;          // Unique order ID
    uint16_t quantity;          // Quantity to trade
    uint16_t filled_quantity;   // Quantity that has been filled
    utils::SymbolLookup::SymbolID
        symbol_id;              // ID of the instrument symbol
    Type type;                  // Order type
    Side side;                  // BUY or SELL
    Status status;              // Order status

        // Parameter struct for Order constructor to avoid parameter confusion
    struct OrderParams {
        uint32_t order_id = 0;
        utils::SymbolLookup::SymbolID symbol_id = utils::SymbolLookup::INVALID_SYMBOL_ID;
        Type type = Type::MARKET;
        Side side = Side::BUY;
        double quantity = 0.0;
        Price price = Price();
        Status status = Status::PENDING;
        double filled_quantity = 0.0;
        std::string timestamp = "";
    };
    
    // Constructor using the parameter struct
    explicit Order(const OrderParams& params);

    // Calculate the remaining quantity to be filled
    double get_remaining_quantity() const;

    // Check if the order is active (pending or partially filled)
    bool is_active() const;

    // Convert order type to string
    std::string type_to_string() const;

    // Convert order side to string
    std::string side_to_string() const;

    // Convert order status to string
    std::string status_to_string() const;
};

}  // namespace core
}  // namespace thales
