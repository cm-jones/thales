#ifndef THALES_CORE_ORDER_HPP
#define THALES_CORE_ORDER_HPP

#include <string>

namespace thales {
namespace core {

/**
 * @struct Order
 * @brief Represents an order to buy or sell a financial instrument.
 */
struct Order {
    enum class Type { MARKET, LIMIT, STOP, STOP_LIMIT };

    enum class Side { BUY, SELL };

    enum class Status { PENDING, FILLED, PARTIALLY_FILLED, CANCELED, REJECTED };

    std::string order_id;       // Unique order ID
    std::string symbol;         // symbol of the instrument
    Type type;                  // Order type
    Side side;                  // Buy or sell
    uint16_t quantity;          // Quantity to trade
    double price;               // Limit price (if applicable)
    double stop_price;          // Stop price (if applicable)
    Status status;              // Order status
    uint16_t filled_quantity;   // Quantity that has been filled
    double average_fill_price;  // Average fill price
    std::string timestamp;      // Timestamp when the order was created

    // Constructor
    Order(const std::string& id = "", const std::string& sym = "",
          Type t = Type::MARKET, Side s = Side::BUY, double qty = 0.0,
          double p = 0.0, double stop_p = 0.0, Status stat = Status::PENDING,
          double filled_qty = 0.0, double avg_fill_price = 0.0,
          const std::string& ts = "");

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

#endif  // THALES_CORE_ORDER_HPP
