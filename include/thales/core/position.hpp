// SPDX-License-Identifier: MIT

#pragma once

// Standard library includes
#include <string>

// Project includes
#include <thales/core/option.hpp>

namespace thales {
namespace core {

/// Parameters for constructing a Position
///
/// Groups all position parameters into a single struct to:
/// - Avoid parameter confusion in constructors
/// - Provide meaningful default values
/// - Allow for future extensibility
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

/// Trading position in a financial instrument
///
/// Represents a complete position with:
/// - Instrument identification (option details)
/// - Position size and pricing
/// - Profit/loss tracking
///
/// All monetary values are stored as doubles for precision.
/// Memory aligned for optimal cache usage.
struct alignas(CACHE_LINE_SIZE) Position {
    Option option;         ///< Associated financial instrument
    double average_price;  ///< Average entry price per unit
    double last_price;     ///< Current market price
    double unrealized_pnl; ///< Current paper profit/loss
    double realized_pnl;   ///< Actual booked profit/loss
    uint16_t quantity;     ///< Number of units held

    /// Construct a Position with specific parameters
    explicit Position(const PositionParams &params = PositionParams{});

    /// Legacy constructor preserved for compatibility
    /// @deprecated Use PositionParams constructor instead
    Position(utils::SymbolLookup::SymbolID id, const std::string &exchange,
             Option::Type option_type, int qty = 0, double avg_price = 0.0,
             double curr_price = 0.0, double unrealized_pnl = 0.0,
             double realized_pnl = 0.0);

    /// Calculate total position market value
    /// @return Current value (quantity * last_price)
    double get_value() const;

    /// Calculate current unrealized profit/loss
    /// @return Paper P&L based on current market price
    double get_unrealized_pnl() const;
};

} // namespace core
} // namespace thales
