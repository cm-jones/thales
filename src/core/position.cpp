#include <thales/core/position.h>

namespace thales {
namespace core {

Position::Position(const std::string& sym, int qty, double avg_price, 
                  double curr_price, double unrealized_pnl, double realized_pnl)
    : symbol(sym),
      quantity(qty),
      average_price(avg_price),
      current_price(curr_price),
      unrealized_pnl(unrealized_pnl),
      realized_pnl(realized_pnl) {}

double Position::get_value() const {
    return quantity * current_price;
}

double Position::calculate_unrealized_pnl() const {
    if (quantity == 0) {
        return 0.0;
    }
    return quantity * (current_price - average_price);
}

} // namespace core
} // namespace thales
