#ifndef THALES_CORE_PORTFOLIO_H
#define THALES_CORE_PORTFOLIO_H

#include <thales/core/order.h>
#include <thales/core/position.h>
#include <thales/utils/config.h>

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace thales {
namespace core {

/**
 * @class Portfolio
 * @brief Manages the portfolio of positions and orders.
 *
 * This class is responsible for tracking positions, orders, and
 * calculating portfolio metrics such as value, profit/loss, etc.
 */
class Portfolio {
   public:
    /**
     * @brief Constructor
     * @param config The configuration for the portfolio
     */
    explicit Portfolio(const utils::Config& config);

    /**
     * @brief Destructor
     */
    ~Portfolio();

    /**
     * @brief Initialize the portfolio
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get all positions in the portfolio
     * @return A vector of positions
     */
    std::vector<Position> get_positions() const;

    /**
     * @brief Get a specific position by symbol
     * @param symbol The symbol to look up
     * @return The position, or an empty position if not found
     */
    Position get_position(const std::string& symbol) const;

    /**
     * @brief Get all open orders
     * @return A vector of open orders
     */
    std::vector<Order> get_open_orders() const;

    /**
     * @brief Get all orders for a specific symbol
     * @param symbol The symbol to look up
     * @return A vector of orders for the symbol
     */
    std::vector<Order> get_orders(const std::string& symbol) const;

    /**
     * @brief Get the total portfolio value
     * @return The total value of all positions
     */
    double get_total_value() const;

    /**
     * @brief Get the total unrealized profit/loss
     * @return The total unrealized profit/loss
     */
    double get_total_unrealized_pnl() const;

    /**
     * @brief Get the total realized profit/loss
     * @return The total realized profit/loss
     */
    double get_total_realized_pnl() const;

    /**
     * @brief Update a position with new market data
     * @param symbol The symbol to update
     * @param current_price The current market price
     */
    void update_position(const std::string& symbol, double current_price);

    /**
     * @brief Add a new position to the portfolio
     * @param position The position to add
     */
    void add_position(const Position& position);

    /**
     * @brief Update an order status
     * @param order_id The order ID to update
     * @param status The new status
     * @param filled_quantity The filled quantity
     * @param average_fill_price The average fill price
     */
    void update_order(const std::string& order_id, Order::Status status,
                      double filled_quantity = 0.0,
                      double average_fill_price = 0.0);

    /**
     * @brief Add a new order to the portfolio
     * @param order The order to add
     */
    void add_order(const Order& order);

    /**
     * @brief Cancel an order
     * @param order_id The order ID to cancel
     * @return true if the order was canceled, false otherwise
     */
    bool cancel_order(const std::string& order_id);

   private:
    // Configuration
    utils::Config config_;

    // Positions and orders
    std::unordered_map<std::string, Position> positions_;
    std::unordered_map<std::string, Order> orders_;

    // Mutex for thread safety
    mutable std::mutex mutex_;

    // Private methods
    void update_position_from_order(const Order& order);
};

}  // namespace core
}  // namespace thales

#endif  // THALES_CORE_PORTFOLIO_H
