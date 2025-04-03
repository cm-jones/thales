#include <thales/core/position.hpp>

namespace thales {
namespace core {

Position::Position(const std::string& sym, int qty, double avg_price,
                   double curr_price, double unrealized_pnl,
                   double realized_pnl)
    : symbol(sym),
      quantity(qty),
      average_price(avg_price),
      last_price(curr_price),
      unrealized_pnl(unrealized_pnl),
      realized_pnl(realized_pnl) {}

double Position::get_value() const { return quantity * last_price; }

double Position::get_unrealized_pnl() const {
    return quantity * (last_price - average_price);
}

}  // namespace core
}  // namespace thales
