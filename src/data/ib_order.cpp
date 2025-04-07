#include <thales/data/ib_order.hpp>
#include <sstream>

namespace thales {
namespace data {

IBOrder::IBOrder(const std::string& action, double total_quantity, const std::string& order_type)
    : action(action), total_quantity(total_quantity), order_type(order_type) {}

std::string IBOrder::to_string() const {
    std::stringstream ss;
    ss << "Order: ";
    
    if (!order_id.empty()) {
        ss << order_id << " ";
    }
    
    ss << action << " " << total_quantity;
    
    if (!order_type.empty()) {
        ss << " " << order_type;
    }
    
    if (order_type == "LMT" && lmt_price > 0.0) {
        ss << " @ " << lmt_price;
    } else if (order_type == "STP" && aux_price > 0.0) {
        ss << " @ " << aux_price;
    }
    
    if (!tif.empty()) {
        ss << " " << tif;
    }
    
    return ss.str();
}

IBOrderState::IBOrderState(const std::string& status)
    : status(status) {}

std::string IBOrderState::to_string() const {
    std::stringstream ss;
    ss << "Status: " << status;
    
    if (commission > 0.0) {
        ss << ", Commission: " << commission;
        if (!commission_currency.empty()) {
            ss << " " << commission_currency;
        }
    }
    
    if (!warning_text.empty()) {
        ss << ", Warning: " << warning_text;
    }
    
    return ss.str();
}

IBExecution::IBExecution(const std::string& exec_id, const std::string& order_id, double shares, double price)
    : exec_id(exec_id), order_id(order_id), shares(shares), price(price) {}

std::string IBExecution::to_string() const {
    std::stringstream ss;
    ss << "Execution: " << exec_id;
    
    if (!order_id.empty()) {
        ss << " for order " << order_id;
    }
    
    if (!side.empty()) {
        ss << " " << side;
    }
    
    ss << " " << shares << " @ " << price;
    
    if (!exchange.empty()) {
        ss << " on " << exchange;
    }
    
    if (!time.empty()) {
        ss << " at " << time;
    }
    
    return ss.str();
}

} // namespace data
} // namespace thales
