#ifndef THALES_CORE_POSITION_H
#define THALES_CORE_POSITION_H

#include <string>

namespace thales {
namespace core {

/**
 * @struct Position
 * @brief Represents a position in a financial instrument.
 */
struct Position {
    std::string symbol;      // Symbol of the instrument
    int quantity;            // Quantity held
    double average_price;    // Average entry price
    double current_price;    // Current market price
    double unrealized_pnl;   // Unrealized profit/loss
    double realized_pnl;     // Realized profit/loss

    // Constructor
    Position(const std::string& sym = "", int qty = 0, double avg_price = 0.0,
             double curr_price = 0.0, double unrealized_pnl = 0.0,
             double realized_pnl = 0.0);

    // Calculate the current value of the position
    double get_value() const;

    // Calculate the unrealized profit/loss
    double calculate_unrealized_pnl() const;
};

}  // namespace core
}  // namespace thales

#endif  // THALES_CORE_POSITION_H
