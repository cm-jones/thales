// SPDX-License-Identifier: MIT

// Project includes
#include "thales/core/order.hpp"

namespace thales {
namespace core {

Order::Order(const OrderParams &params)
    : timestamp(params.timestamp),
      price(params.price),
      order_id(params.order_id),
      quantity(static_cast<uint16_t>(params.quantity)),
      filled_quantity(static_cast<uint16_t>(params.filled_quantity)),
      symbol_id(params.symbol_id),
      type(params.type),
      side(params.side),
      status(params.status) {}

double Order::get_remaining_quantity() const {
    return static_cast<double>(quantity - filled_quantity);
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
