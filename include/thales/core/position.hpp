#pragma once

#include <string>
#include <thales/core/contract.hpp>

namespace thales {
namespace core {

/**
 * @struct Position
 * @brief Represents a position in a financial instrument.
 */
struct Position {
    Contract contract;         // The instrument associated with the position (varies,
                           // >64 bytes)
    double average_price;  // Average entry price (4 bytes)
    double last_price;     // Current market price (4 bytes)
    double unrealized_pnl;  // Unrealized profit/loss (4 bytes)
    double realized_pnl;    // Realized profit/loss (4 bytes)
    uint16_t quantity;      // Quantity held (2 bytes)

    // Constructor
    Position(utils::SymbolLookup::symbol_id_t sym_id =
                 utils::SymbolLookup::INVALID_SYMBOL_ID,
             const std::string& exchange = "",
             Contract::Type option_type = Contract::Type::UNKNOWN, int qty = 0,
             double avg_price = 0.0, double curr_price = 0.0,
             double unrealized_pnl = 0.0, double realized_pnl = 0.0);

    // Calculate the current value of the position
    double get_value() const;

    // Calculate the unrealized profit/loss
    double get_unrealized_pnl() const;
};

}  // namespace core
}  // namespace thales
