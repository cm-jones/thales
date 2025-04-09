#pragma once

#include <string>
#include <thales/utils/symbol_lookup.hpp>

namespace thales {
namespace core {

/**
 * @struct Option
 * @brief Represents an options contract
 */
struct alignas(64) Option {
    /**
     * @enum Type
     * @brief Defines the type of the option: CALL or PUT.
     */
    enum struct Type : uint8_t {
        CALL,     // Call option
        PUT,      // Put option
        UNKNOWN,  // Used for initialization or error handling
    };

    /**
     * @struct Greeks
     * @brief Contains the Greeks of the option: delta, gamma, theta, vega,
     * rho.
     */
    struct alignas(64) Greeks {
        double delta = 0.0;  // Delta of the option
        double gamma = 0.0;  // Gamma of the option
        double theta = 0.0;  // Theta of the option
        double vega = 0.0;   // Vega of the option
        double rho = 0.0;    // Rho of the option

        Greeks() = default;
    };

    std::unique_ptr<Greeks> greeks;           // Greeks of the option
    std::array<char, 8> exchange;                         // The exchange where the option is traded
    std::array<char, 8> expiry;                           // Expiration date
    double strike;                            // Strike price
    utils::SymbolLookup::SymbolID symbol_id;  // The ID of the symbol
    Type type;                                // CALL or PUT

    // Constructor
    Option(utils::SymbolLookup::SymbolID sym_id =
               utils::SymbolLookup::INVALID_SYMBOL_ID,
           const std::string& exch = "", Type t = Type::UNKNOWN,
           const std::string& exp = "", double strk = 0.0)
        : exchange(exch),
          expiry(exp),
          strike(strk),
          type(t),
          symbol_id(sym_id) {}
};

}  // namespace core
}  // namespace thales
