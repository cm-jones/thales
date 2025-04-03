#include <thales/core/order.hpp>

namespace thales {
namespace core {

Order::Order(const std::string& id, const std::string& sym, Type t, Side s,
             double qty, double p, double stop_p, Status stat,
             double filled_qty, double avg_fill_price, const std::string& ts)
    : order_id(id),
      symbol(sym),
      type(t),
      side(s),
      quantity(qty),
      price(p),
      stop_price(stop_p),
      status(stat),
      filled_quantity(filled_qty),
      average_fill_price(avg_fill_price),
      timestamp(ts) {}

double Order::get_remaining_quantity() const {
    return quantity - filled_quantity;
}

bool Order::is_active() const {
    return status == Status::PENDING || status == Status::PARTIALLY_FILLED;
}

std::string Order::type_to_string() const {
    switch (type) {
        case Type::MARKET:
            return "MARKET";
        case Type::LIMIT:
            return "LIMIT";
        case Type::STOP:
            return "STOP";
        case Type::STOP_LIMIT:
            return "STOP_LIMIT";
        default:
            return "UNKNOWN";
    }
}

std::string Order::side_to_string() const {
    switch (side) {
        case Side::BUY:
            return "BUY";
        case Side::SELL:
            return "SELL";
        default:
            return "UNKNOWN";
    }
}

std::string Order::status_to_string() const {
    switch (status) {
        case Status::PENDING:
            return "PENDING";
        case Status::FILLED:
            return "FILLED";
        case Status::PARTIALLY_FILLED:
            return "PARTIALLY_FILLED";
        case Status::CANCELED:
            return "CANCELED";
        case Status::REJECTED:
            return "REJECTED";
        default:
            return "UNKNOWN";
    }
}

}  // namespace core
}  // namespace thales
