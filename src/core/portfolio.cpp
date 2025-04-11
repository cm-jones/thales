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

boost::container::static_vector<Positions, 256> Portfolio::get_positions() const {
    std::lock_guard<std::mutex> lock(mutex_);
    boost::container::static_vector<Positions, 256> result;
    result.reserve(positions_.size());

    for (const auto& pos : positions_) {
        // Create new position with same data but new Greeks instance
        Position new_pos(PositionParams{
            .id = pos.option.symbol_id,
            .exchange = utils::SymbolLookup::get_instance().get_symbol(pos.option.symbol_id),
            .option_type = pos.option.type,
            .quantity = pos.quantity,
            .avg_price = pos.average_price,
            .curr_price = pos.last_price,
            .unrealized_pnl = pos.unrealized_pnl,
            .realized_pnl = pos.realized_pnl
        });
        result.push_back(std::move(new_pos));
    }

    return result;
}

Position Portfolio::get_position(const std::string& symbol) const {
    std::lock_guard<std::mutex> lock(mutex_);

    // Convert symbol string to symbol_id using SymbolLookup
    utils::SymbolLookup::SymbolID symbol_id = utils::SymbolLookup::get_instance().get_id(symbol);
    
    // Find position by symbol_id
    for (const auto& position : positions_) {
        if (position.option.symbol_id == symbol_id) {
            Position new_pos(PositionParams{
                .id = position.option.symbol_id,
                .exchange = symbol,
                .option_type = position.option.type,
                .quantity = position.quantity,
                .avg_price = position.average_price,
                .curr_price = position.last_price,
                .unrealized_pnl = position.unrealized_pnl,
                .realized_pnl = position.realized_pnl
            });
            return new_pos;
        }
    }

    return Position();  // Return an empty position if not found
}

boost::container::static_vector<Order, 256> Portfolio::get_open_orders() const {
    std::lock_guard<std::mutex> lock(mutex_);
    boost::container::static_vector<Order, 256> result;

    for (const auto& order : orders_) {
        if (order.is_active()) {
            result.push_back(order);  // Order is copyable, no need for move
        }
    }

    return result;
}

boost::container::static_vector<Order, 256> Portfolio::get_orders(const std::string& symbol) const {
    std::lock_guard<std::mutex> lock(mutex_);
    boost::container::static_vector<Order, 256> result;

    for (const auto& order : orders_) {
        std::string order_symbol = utils::SymbolLookup::get_instance().get_symbol(order.symbol_id);
        if (order_symbol == symbol) {
            result.push_back(order);  // Order is copyable, no need for move
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
    utils::SymbolLookup::SymbolID symbol_id = utils::SymbolLookup::get_instance().get_id(symbol);
    
    // Find position by symbol_id and update it
    for (auto& position : positions_) {
        if (position.option.symbol_id == symbol_id) {
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
                          [&symbol_id = position.option.symbol_id](const Position& p) {
                              return p.option.symbol_id == symbol_id;
                          });
    
    if (it != positions_.end()) {
        // Update existing position data without copying unique_ptr
        it->quantity = position.quantity;
        it->average_price = position.average_price;
        it->last_price = position.last_price;
        it->unrealized_pnl = position.unrealized_pnl;
        it->realized_pnl = position.realized_pnl;
    } else {
        // Create a new position with same data
        Position new_pos(PositionParams{
            .id = position.option.symbol_id,
            .exchange = utils::SymbolLookup::get_instance().get_symbol(position.option.symbol_id),
            .option_type = position.option.type,
            .quantity = position.quantity,
            .avg_price = position.average_price,
            .curr_price = position.last_price,
            .unrealized_pnl = position.unrealized_pnl,
            .realized_pnl = position.realized_pnl
        });
        positions_.push_back(std::move(new_pos));
    }
}

void Portfolio::update_order(const std::string& order_id, Order::Status status,
                             double filled_quantity,
                             double average_fill_price) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Convert string order_id to uint32_t
    uint32_t numeric_order_id = std::stoul(order_id);

    auto it = std::find_if(orders_.begin(), orders_.end(),
                          [numeric_order_id](const Order& o) {
                              return o.order_id == numeric_order_id;
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
                order.price.average_fill =
                    (order.price.average_fill * order.filled_quantity +
                     average_fill_price * filled_quantity) /
                    new_filled;
            } else {
                order.price.average_fill = average_fill_price;
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
                          [id = order.order_id](const Order& o) {
                              return o.order_id == id;
                          });
    
    if (it != orders_.end()) {
        *it = order;  // Order is copyable
    } else {
        orders_.push_back(order);  // Order is copyable
    }
}

bool Portfolio::cancel_order(const std::string& order_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Convert string order_id to uint32_t
    uint32_t numeric_order_id = std::stoul(order_id);

    auto it = std::find_if(orders_.begin(), orders_.end(),
                          [numeric_order_id](const Order& o) {
                              return o.order_id == numeric_order_id;
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
    auto it = std::find_if(positions_.begin(), positions_.end(),
                          [&symbol_id = order.symbol_id](const Position& p) {
                              return p.option.symbol_id == symbol_id;
                          });
    
    if (it == positions_.end()) {
        // Create a new position if we don't have one
        Position new_pos(PositionParams{.id = order.symbol_id});
        positions_.push_back(std::move(new_pos));
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
                                    order.price.average_fill * filled) /
                                    position.quantity;
        } else {
            // New position or flipping from short to long
            position.average_price = order.price.average_fill;
        }
    }

    // Update the unrealized P&L
    position.unrealized_pnl = position.get_unrealized_pnl();
}

}  // namespace core
}  // namespace thales
