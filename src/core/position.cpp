#include <thales/core/position.hpp>

namespace thales {
namespace core {

Position::Position(const PositionParams& params)
    : option(params.id, params.exchange, params.option_type),
      average_price(params.avg_price),
      last_price(params.curr_price),
      unrealized_pnl(params.unrealized_pnl),
      realized_pnl(params.realized_pnl),
      quantity(static_cast<uint16_t>(params.quantity)) {}

Position::Position(utils::SymbolLookup::SymbolID id,
                  const std::string& exchange,
                  Option::Type option_type,
                  int qty,
                  double avg_price,
                  double curr_price,
                  double unrealized_pnl,
                  double realized_pnl)
    : Position(PositionParams{
        .id = id,
        .exchange = exchange,
        .option_type = option_type,
        .quantity = qty,
        .avg_price = avg_price,
        .curr_price = curr_price,
        .unrealized_pnl = unrealized_pnl,
        .realized_pnl = realized_pnl
    }) {}

double Position::get_value() const { return quantity * last_price; }

double Position::get_unrealized_pnl() const {
    return quantity * (last_price - average_price);
}

}  // namespace core
}  // namespace thales
