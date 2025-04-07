#include <algorithm>
#include <thales/core/portfolio.hpp>

namespace thales {
namespace core {

Portfolio::Portfolio(const utils::Config& config) : config_(config) {}

Portfolio::~Portfolio() {}

bool Portfolio::initialize() {
    // Implementation would typically load positions and orders from database
    // or previous session data
    return true;
}

std::vector<Position> Portfolio::get_positions() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Position> result;
    result.reserve(positions_.size());

    for (const auto& position : positions_) {
        result.push_back(position);
    }

    return result;
}

Position Portfolio::get_position(const std::string& symbol) const {
    std::lock_guard<std::mutex> lock(mutex_);

    // Convert symbol string to symbol_id using SymbolLookup
    utils::SymbolLookup::symbol_id_t symbol_id = utils::SymbolLookup::get_instance().get_id(symbol);
    
    // Find position by symbol_id
    for (const auto& position : positions_) {
        if (position.contract.symbol_id == symbol_id) {
            return position;
        }
    }

    return Position();  // Return an empty position if not found
}

std::vector<Order> Portfolio::get_open_orders() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Order> result;

    for (const auto& order : orders_) {
        if (order.is_active()) {
            result.push_back(order);
        }
    }

    return result;
}

std::vector<Order> Portfolio::get_orders(const std::string& symbol) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Order> result;

    for (const auto& order : orders_) {
        std::string order_symbol = utils::SymbolLookup::get_instance().get_symbol(order.symbol_id);
        if (order_symbol == symbol) {
            result.push_back(order);
        }
    }

    return result;
}

double Portfolio::get_total_value() const {
    std::lock_guard<std::mutex> lock(mutex_);
    double total = 0.0;

    for (const auto& position : positions_) {
        total += position.get_value();
    }

    return total;
}

double Portfolio::get_total_unrealized_pnl() const {
    std::lock_guard<std::mutex> lock(mutex_);
    double total = 0.0;

    for (const auto& position : positions_) {
        total += position.unrealized_pnl;
    }

    return total;
}

double Portfolio::get_total_realized_pnl() const {
    std::lock_guard<std::mutex> lock(mutex_);
    double total = 0.0;

    for (const auto& position : positions_) {
        total += position.realized_pnl;
    }

    return total;
}

void Portfolio::update_position(const std::string& symbol, double last_price) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Convert symbol string to symbol_id using SymbolLookup
    utils::SymbolLookup::symbol_id_t symbol_id = utils::SymbolLookup::get_instance().get_id(symbol);
    
    // Find position by symbol_id and update it
    for (auto& position : positions_) {
        if (position.contract.symbol_id == symbol_id) {
            position.last_price = last_price;
            position.unrealized_pnl = position.get_unrealized_pnl();
            break;
        }
    }
}

void Portfolio::add_position(const Position& position) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Find if position already exists
    auto it = std::find_if(positions_.begin(), positions_.end(), 
                          [&symbol_id = position.contract.symbol_id](const Position& p) {
                              return p.contract.symbol_id == symbol_id;
                          });
    
    if (it != positions_.end()) {
        // Update existing position
        *it = position;
    } else {
        // Add new position
        positions_.push_back(position);
    }
}

void Portfolio::update_order(const std::string& order_id, Order::Status status,
                             double filled_quantity,
                             double average_fill_price) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = std::find_if(orders_.begin(), orders_.end(),
                          [&order_id](const Order& o) {
                              return o.order_id == order_id;
                          });
    
    if (it != orders_.end()) {
        Order& order = *it;
        order.status = status;

        if (filled_quantity > 0) {
            double new_filled = order.filled_quantity + filled_quantity;
            if (new_filled > order.quantity) {
                new_filled = order.quantity;
            }

            // Calculate the weighted average fill price
            if (order.filled_quantity > 0) {
                order.average_fill_price =
                    (order.average_fill_price * order.filled_quantity +
                     average_fill_price * filled_quantity) /
                    new_filled;
            } else {
                order.average_fill_price = average_fill_price;
            }

            order.filled_quantity = static_cast<uint16_t>(new_filled);

            // Update the position based on the filled order
            update_position_from_order(order);
        }
    }
}

void Portfolio::add_order(const Order& order) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Find if order already exists
    auto it = std::find_if(orders_.begin(), orders_.end(),
                          [&order_id = order.order_id](const Order& o) {
                              return o.order_id == order_id;
                          });
    
    if (it != orders_.end()) {
        // Update existing order
        *it = order;
    } else {
        // Add new order
        orders_.push_back(order);
    }
}

bool Portfolio::cancel_order(const std::string& order_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = std::find_if(orders_.begin(), orders_.end(),
                          [&order_id](const Order& o) {
                              return o.order_id == order_id;
                          });
    
    if (it != orders_.end() && it->is_active()) {
        it->status = Order::Status::CANCELED;
        return true;
    }

    return false;
}

void Portfolio::update_position_from_order(const Order& order) {
    // Only process filled or partially filled orders
    if (order.status != Order::Status::FILLED &&
        order.status != Order::Status::PARTIALLY_FILLED) {
        return;
    }

    // Find or create position
    // Find position by symbol
    auto it = std::find_if(positions_.begin(), positions_.end(),
                          [&symbol_id = order.symbol_id](const Position& p) {
                              return p.contract.symbol_id == symbol_id;
                          });
    
    if (it == positions_.end()) {
        // Create a new position if we don't have one
        Position position(order.symbol_id);
        positions_.push_back(position);
        it = positions_.end() - 1;  // Point to the newly added position
    }

    // Calculate the filled quantity (positive for buys, negative for sells)
    double filled = order.filled_quantity;
    if (order.side == Order::Side::SELL) {
        filled = -filled;
    }

    // Update the position
    Position& position = *it;
    int old_quantity = position.quantity;
    position.quantity += static_cast<uint16_t>(filled);

    // Update average price (only for buys that increase position)
    if (filled > 0) {
        if (old_quantity > 0) {
            // Weighted average of existing position and new purchase
            position.average_price = (position.average_price * old_quantity +
                                      order.average_fill_price * filled) /
                                     position.quantity;
        } else {
            // New position or flipping from short to long
            position.average_price = order.average_fill_price;
        }
    }

    // For simplicity we don't handle short positions' cost basis in this
    // example

    // Update the unrealized P&L
    position.unrealized_pnl = position.get_unrealized_pnl();

    // For a complete implementation, we would also calculate realized P&L here
    // when reducing positions
}

}  // namespace core
}  // namespace thales
