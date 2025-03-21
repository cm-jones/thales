#include <thales/core/portfolio.h>
#include <algorithm>

namespace thales {
namespace core {

Portfolio::Portfolio(const utils::Config& config)
    : config_(config) {}

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
    
    for (const auto& [_, position] : positions_) {
        result.push_back(position);
    }
    
    return result;
}

Position Portfolio::get_position(const std::string& symbol) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = positions_.find(symbol);
    if (it != positions_.end()) {
        return it->second;
    }
    return Position(); // Return an empty position if not found
}

std::vector<Order> Portfolio::get_open_orders() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Order> result;
    
    for (const auto& [_, order] : orders_) {
        if (order.is_active()) {
            result.push_back(order);
        }
    }
    
    return result;
}

std::vector<Order> Portfolio::get_orders(const std::string& symbol) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Order> result;
    
    for (const auto& [_, order] : orders_) {
        if (order.symbol == symbol) {
            result.push_back(order);
        }
    }
    
    return result;
}

double Portfolio::get_total_value() const {
    std::lock_guard<std::mutex> lock(mutex_);
    double total = 0.0;
    
    for (const auto& [_, position] : positions_) {
        total += position.get_value();
    }
    
    return total;
}

double Portfolio::get_total_unrealized_pnl() const {
    std::lock_guard<std::mutex> lock(mutex_);
    double total = 0.0;
    
    for (const auto& [_, position] : positions_) {
        total += position.unrealized_pnl;
    }
    
    return total;
}

double Portfolio::get_total_realized_pnl() const {
    std::lock_guard<std::mutex> lock(mutex_);
    double total = 0.0;
    
    for (const auto& [_, position] : positions_) {
        total += position.realized_pnl;
    }
    
    return total;
}

void Portfolio::update_position(const std::string& symbol, double current_price) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = positions_.find(symbol);
    if (it != positions_.end()) {
        it->second.current_price = current_price;
        it->second.unrealized_pnl = it->second.calculate_unrealized_pnl();
    }
}

void Portfolio::add_position(const Position& position) {
    std::lock_guard<std::mutex> lock(mutex_);
    positions_[position.symbol] = position;
}

void Portfolio::update_order(const std::string& order_id, Order::Status status,
                           double filled_quantity, double average_fill_price) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = orders_.find(order_id);
    if (it != orders_.end()) {
        Order& order = it->second;
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
                     average_fill_price * filled_quantity) / new_filled;
            } else {
                order.average_fill_price = average_fill_price;
            }
            
            order.filled_quantity = new_filled;
            
            // Update the position based on the filled order
            update_position_from_order(order);
        }
    }
}

void Portfolio::add_order(const Order& order) {
    std::lock_guard<std::mutex> lock(mutex_);
    orders_[order.order_id] = order;
}

bool Portfolio::cancel_order(const std::string& order_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = orders_.find(order_id);
    if (it != orders_.end() && it->second.is_active()) {
        it->second.status = Order::Status::CANCELED;
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
    
    auto it = positions_.find(order.symbol);
    if (it == positions_.end()) {
        // Create a new position if we don't have one
        Position position(order.symbol);
        positions_[order.symbol] = position;
        it = positions_.find(order.symbol);
    }
    
    // Calculate the filled quantity (positive for buys, negative for sells)
    double filled = order.filled_quantity;
    if (order.side == Order::Side::SELL) {
        filled = -filled;
    }
    
    // Update the position
    Position& position = it->second;
    int old_quantity = position.quantity;
    position.quantity += filled;
    
    // Update average price (only for buys that increase position)
    if (filled > 0) {
        if (old_quantity > 0) {
            // Weighted average of existing position and new purchase
            position.average_price = 
                (position.average_price * old_quantity + 
                 order.average_fill_price * filled) / position.quantity;
        } else {
            // New position or flipping from short to long
            position.average_price = order.average_fill_price;
        }
    }
    
    // For simplicity we don't handle short positions' cost basis in this example
    
    // Update the unrealized P&L
    position.unrealized_pnl = position.calculate_unrealized_pnl();
    
    // For a complete implementation, we would also calculate realized P&L here
    // when reducing positions
}

} // namespace core
} // namespace thales
