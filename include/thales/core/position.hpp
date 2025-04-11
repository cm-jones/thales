#pragma once

#include <string>
#include <thales/core/option.hpp>

namespace thales {
namespace core {

/**
 * @struct PositionParams
 * @brief Parameters for constructing a Position
 */
struct PositionParams {
    utils::SymbolLookup::SymbolID id{utils::SymbolLookup::INVALID_SYMBOL_ID};
    std::string exchange{};
    Option::Type option_type{Option::Type::UNKNOWN};
    int quantity{0};
    double avg_price{0.0};
    double curr_price{0.0};
    double unrealized_pnl{0.0};
    double realized_pnl{0.0};
};

/**
 * @struct Position
 * @brief Represents a position in a financial instrument.
 */
struct alignas(CACHE_LINE_SIZE) Position {
    Option option;         // The instrument associated with the position (varies)
    double average_price;  // Average entry price (4 bytes)
    double last_price;     // Current market price (4 bytes)
    double unrealized_pnl;  // Unrealized profit/loss (4 bytes)
    double realized_pnl;    // Realized profit/loss (4 bytes)
    uint16_t quantity;      // Quantity held (2 bytes)

    // Constructor
    explicit Position(const PositionParams& params = PositionParams{});
    
    // Legacy constructor for backward compatibility
    Position(utils::SymbolLookup::SymbolID id,
             const std::string& exchange,
             Option::Type option_type, 
             int qty = 0,
             double avg_price = 0.0, 
             double curr_price = 0.0,
             double unrealized_pnl = 0.0, 
             double realized_pnl = 0.0);

    // Calculate the current value of the position
    double get_value() const;

    // Calculate the unrealized profit/loss
    double get_unrealized_pnl() const;
};

}  // namespace core
}  // namespace thales
