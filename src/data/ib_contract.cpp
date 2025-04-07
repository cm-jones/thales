#include <thales/data/ib_contract.hpp>
#include <sstream>

namespace thales {
namespace data {

IBContract::IBContract(const std::string& symbol, const std::string& sec_type,
                     const std::string& exchange, const std::string& currency)
    : symbol(symbol), sec_type(sec_type), exchange(exchange), currency(currency) {}

std::string IBContract::to_string() const {
    std::stringstream ss;
    ss << "Contract: " << symbol;
    
    if (!sec_type.empty()) {
        ss << " (" << sec_type << ")";
    }
    
    if (!exchange.empty()) {
        ss << " @ " << exchange;
    }
    
    if (!currency.empty()) {
        ss << " in " << currency;
    }
    
    if (contract_id > 0) {
        ss << " [ID: " << contract_id << "]";
    }
    
    if (sec_type == "OPT" || sec_type == "FOP") {
        ss << " " << strike;
        if (!right.empty()) {
            ss << " " << right;
        }
        if (!last_trade_date.empty()) {
            ss << " " << last_trade_date;
        }
    }
    
    return ss.str();
}

IBContractDetails::IBContractDetails(const IBContract& contract)
    : contract(contract) {}

std::string IBContractDetails::to_string() const {
    std::stringstream ss;
    ss << contract.to_string() << "\n";
    
    if (!market_name.empty()) {
        ss << "Market: " << market_name << "\n";
    }
    
    if (!long_name.empty()) {
        ss << "Name: " << long_name << "\n";
    }
    
    if (!industry.empty()) {
        ss << "Industry: " << industry << "\n";
    }
    
    if (!category.empty()) {
        ss << "Category: " << category << "\n";
    }
    
    if (!subcategory.empty()) {
        ss << "Subcategory: " << subcategory << "\n";
    }
    
    if (min_tick > 0.0) {
        ss << "Min Tick: " << min_tick << "\n";
    }
    
    if (!trading_hours.empty()) {
        ss << "Trading Hours: " << trading_hours << "\n";
    }
    
    if (!liquid_hours.empty()) {
        ss << "Liquid Hours: " << liquid_hours << "\n";
    }
    
    if (!time_zone_id.empty()) {
        ss << "Time Zone: " << time_zone_id << "\n";
    }
    
    return ss.str();
}

IBContractDescription::IBContractDescription(const IBContract& contract)
    : contract(contract) {}

std::string IBContractDescription::to_string() const {
    std::stringstream ss;
    ss << contract.to_string() << "\n";
    
    if (!derivative_sec_types.empty()) {
        ss << "Derivative Types: ";
        for (size_t i = 0; i < derivative_sec_types.size(); ++i) {
            if (i > 0) {
                ss << ", ";
            }
            ss << derivative_sec_types[i];
        }
        ss << "\n";
    }
    
    return ss.str();
}

} // namespace data
} // namespace thales
