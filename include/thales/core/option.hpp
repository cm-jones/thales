#pragma once

#include <string>
#include <thales/utils/symbol_lookup.hpp>

namespace thales {
namespace core {

/**
 * @struct Option
 * @brief Represents an option contract
 */
struct Option {
        /**
         * @enum Type
         * @brief Defines the type of the option: CALL or PUT.
         */
        enum struct Type : uint8_t {
            CALL,     // Call option
            PUT,      // Put option
            UNKNOWN,  // Unknown option type
        };

        /**
         * @struct Greeks
         * @brief Contains the Greeks of the option: delta, gamma, theta, vega,
         * rho.
         */
        struct Greeks {
                double delta = 0.0;  // Delta of the option
                double gamma = 0.0;  // Gamma of the option
                double theta = 0.0;  // Theta of the option
                double vega = 0.0;   // Vega of the option
                double rho = 0.0;    // Rho of the option

                Greeks() = default;
        };

        std::string
            exchange;  // The exchange where the option is traded (24-32 bytes)
        std::string expiry;  // Expiration date (24-32 bytes)
        double strike;       // Strike price (for options) (8 bytes)
        Type option_type;    // Type of the option (CALL or PUT) (1 byte)
        utils::SymbolLookup::symbol_id_t
            symbol_id;  // The ID of the symbol (2 bytes)

        // Constructor
        Option(utils::SymbolLookup::symbol_id_t sym_id =
                   utils::SymbolLookup::INVALID_SYMBOL_ID,
               const std::string& exch = "", Type t = Type::UNKNOWN,
               const std::string& exp = "", double strk = 0.0)
            : exchange(exch),
              expiry(exp),
              strike(strk),
              option_type(t),
              symbol_id(sym_id) {}
};

}  // namespace core
}  // namespace thales
