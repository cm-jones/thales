#pragma once

#include <string>
#include <thales/utils/symbol_lookup.hpp>

namespace thales {
namespace core {

/**
 * @struct Order
 * @brief Represents an order to buy or sell a financial instrument.
 */
struct Order {
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

        std::string order_id;  // Unique order ID (24-32 bytes)
        std::string
            timestamp;  // Timestamp when the order was created (24-32 bytes)
        double price;   // Limit price (if applicable) (8 bytes)
        double stop_price;          // Stop price (if applicable) (8 bytes)
        double average_fill_price;  // Average fill price (8 bytes)
        uint16_t quantity;          // Quantity to trade (2 bytes)
        uint16_t filled_quantity;   // Quantity that has been filled (2 bytes)
        utils::SymbolLookup::symbol_id_t
            symbol_id;  // ID of the instrument symbol (2 bytes)
        Type type;      // Order type (1 byte)
        Side side;      // Buy or sell (1 byte)
        Status status;  // Order status (1 byte)

        // Constructor
        Order(const std::string& id = "",
              utils::SymbolLookup::symbol_id_t sym_id =
                  utils::SymbolLookup::INVALID_SYMBOL_ID,
              Type t = Type::MARKET, Side s = Side::BUY, double qty = 0.0,
              double p = 0.0, double stop_p = 0.0,
              Status stat = Status::PENDING, double filled_qty = 0.0,
              double avg_fill_price = 0.0, const std::string& ts = "");

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
