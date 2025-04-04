#include <thales/utils/symbol_lookup.hpp>

namespace thales {
namespace utils {

SymbolLookup::SymbolLookup(const std::vector<std::string>& symbols) {
    for (const auto& symbol : symbols) {
        add_symbol(symbol);
    }
}

SymbolLookup::symbol_id_t SymbolLookup::add_symbol(const std::string& symbol) {
    // If the symbol already exists, return its ID
    if (has_symbol(symbol)) {
        return symbol_to_id_[symbol];
    }

    // Assign a new ID to the symbol
    symbol_id_t id = next_id_++;
    symbol_to_id_[symbol] = id;
    id_to_symbol_[id] = symbol;
    return id;
}

SymbolLookup::symbol_id_t SymbolLookup::get_id(
    const std::string& symbol) const {
    auto it = symbol_to_id_.find(symbol);
    if (it != symbol_to_id_.end()) {
        return it->second;
    }
    return INVALID_SYMBOL_ID;
}

std::string SymbolLookup::get_symbol(symbol_id_t id) const {
    auto it = id_to_symbol_.find(id);
    if (it != id_to_symbol_.end()) {
        return it->second;
    }
    return "";
}

bool SymbolLookup::has_symbol(const std::string& symbol) const {
    return symbol_to_id_.find(symbol) != symbol_to_id_.end();
}

bool SymbolLookup::has_id(symbol_id_t id) const {
    return id_to_symbol_.find(id) != id_to_symbol_.end();
}

size_t SymbolLookup::size() const { return symbol_to_id_.size(); }

std::vector<std::string> SymbolLookup::get_all_symbols() const {
    std::vector<std::string> symbols;
    symbols.reserve(symbol_to_id_.size());

    for (const auto& pair : symbol_to_id_) {
        symbols.push_back(pair.first);
    }

    return symbols;
}

}  // namespace utils
}  // namespace thales
