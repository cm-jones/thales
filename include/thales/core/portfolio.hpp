// SPDX-License-Identifier: MIT

#pragma once

#define CACHE_LINE_SIZE 64

// Standard library includes
#include <mutex>
#include <string>

// Third-party includes
#include <boost/container/static_vector.hpp>

// Project includes
#include "option.hpp"
#include "order.hpp"
#include "position.hpp"
#include "thales/utils/config.hpp"
#include "thales/utils/symbol_lookup.hpp"

namespace thales {
namespace core {

/// Portfolio management system responsible for tracking positions and orders.
///
/// Provides thread-safe management of:
/// - Active positions and their current market values
/// - Open and historical orders
/// - Portfolio metrics (value, P&L)
/// - Position updates from market data and fills
///
/// Uses static vectors with pre-allocated capacity for performance.
class CACHE_ALIGNED Portfolio {
   public:
    /// Construct a new Portfolio instance
    /// @param config Portfolio configuration parameters
    explicit Portfolio(const utils::Config &config);

    /// @brief Destructor
    ~Portfolio();

    /// Initialize the portfolio state
    /// @return true if initialization succeeds, false otherwise
    bool initialize();

    /// Retrieve all current positions
    /// @return Vector of active positions (thread-safe copy)
    boost::container::static_vector<Position, 256> get_positions() const;

    /// Lookup a specific position by symbol
    /// @param symbol Trading symbol to search for
    /// @return Position for the symbol, or empty position if not found
    Position get_position(const std::string &symbol) const;

    /// Retrieve all open orders
    /// @return Vector of currently open orders (thread-safe copy)
    boost::container::static_vector<Order, 256> get_open_orders() const;

    /// Lookup all orders for a specific symbol
    /// @param symbol Trading symbol to search for
    /// @return Vector of all orders for the symbol (thread-safe copy)
    boost::container::static_vector<Order, 256> get_orders(
        const std::string &symbol) const;

    /// Calculate total portfolio market value
    /// @return Sum of all position values in portfolio
    double get_total_value() const;

    /// Calculate total unrealized profit/loss
    /// @return Sum of unrealized P&L across all positions
    double get_total_unrealized_pnl() const;

    /// Calculate total realized profit/loss
    /// @return Sum of realized P&L from closed positions
    double get_total_realized_pnl() const;

    /// Update position with new market data
    /// @param symbol Symbol of position to update
    /// @param last_price Current market price
    void update_position(const std::string &symbol, double last_price);

    /// Add new position to portfolio
    /// @param position Position to add
    void add_position(const Position &position);

    /// Update order execution status
    /// @param order_id ID of order to update
    /// @param status New order status
    /// @param filled_quantity Quantity filled in this update
    /// @param average_fill_price Average price of fills
    void update_order(const std::string &order_id, Order::Status status,
                      double filled_quantity = 0.0,
                      double average_fill_price = 0.0);

    /// Submit new order to portfolio
    /// @param order Order to add
    void add_order(const Order &order);

    /// Cancel existing order
    /// @param order_id ID of order to cancel
    /// @return true if order was found and canceled, false otherwise
    bool cancel_order(const std::string &order_id);

   private:
    // Core data structures
    CACHE_ALIGNED boost::container::static_vector<Position, 256>
        positions_;  ///< Active positions

    CACHE_ALIGNED
    boost::container::static_vector<Order, 256> orders_;  ///< Order history

    utils::Config config_;      ///< Portfolio configuration
    mutable std::mutex mutex_;  ///< Synchronization guard

    /// Update position based on order execution
    /// @param order Order that was executed
    void update_position_from_order(const Order &order);
};

}  // namespace core
}  // namespace thales
