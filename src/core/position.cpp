#include <thales/core/position.hpp>

namespace thales {
namespace core {

Position::Position(utils::SymbolLookup::symbol_id_t sym_id,
                   const std::string& exchange,
                   Contract::Type option_type, int qty,
                   double avg_price, double curr_price,
                   double unrealized_pnl, double realized_pnl)
    : contract(sym_id, exchange, option_type),
      average_price(avg_price),
      last_price(curr_price),
      unrealized_pnl(unrealized_pnl),
      realized_pnl(realized_pnl),
      quantity(static_cast<uint16_t>(qty)) {}

double Position::get_value() const { return quantity * last_price; }

double Position::get_unrealized_pnl() const {
    return quantity * (last_price - average_price);
}

}  // namespace core
}  // namespace thales
