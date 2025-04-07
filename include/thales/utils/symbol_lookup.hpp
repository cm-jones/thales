#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace thales {
namespace utils {

/**
 * @class SymbolLookup
 * @brief Provides a bidirectional mapping between symbol strings and integer
 * IDs.
 *
 * This class implements the singleton pattern to ensure there is only one
 * instance of the lookup table throughout the application. It allows efficient
 * storage and lookup of trading symbols by mapping string symbols to integer
 * IDs and vice versa.
 */
class SymbolLookup {
   public:
    using symbol_id_t = uint16_t;
    static constexpr symbol_id_t INVALID_SYMBOL_ID = 0;

    /**
     * @brief Delete copy constructor to prevent duplication
     */
    SymbolLookup(const SymbolLookup&) = delete;

    /**
     * @brief Delete assignment operator to prevent duplication
     */
    SymbolLookup& operator=(const SymbolLookup&) = delete;

    /**
     * @brief Get the singleton instance of the SymbolLookup
     * @return Reference to the singleton instance
     */
    static SymbolLookup& get_instance();

    /**
     * @brief Initialize the singleton with a list of symbols
     * @param symbols Vector of symbol strings to initialize with
     * @note This should be called only once at application startup
     */
    static void initialize(const std::vector<std::string>& symbols);

    /**
     * @brief Add a symbol to the lookup table
     * @param symbol The symbol string to add
     * @return The ID assigned to the symbol
     */
    symbol_id_t add_symbol(const std::string& symbol);

    /**
     * @brief Get the ID for a given symbol
     * @param symbol The symbol string to look up
     * @return The ID of the symbol, or INVALID_SYMBOL_ID if not found
     */
    symbol_id_t get_id(const std::string& symbol) const;

    /**
     * @brief Get the symbol string for a given ID
     * @param id The ID to look up
     * @return The symbol string, or empty string if not found
     */
    std::string get_symbol(symbol_id_t id) const;

    /**
     * @brief Check if a symbol exists in the lookup table
     * @param symbol The symbol string to check
     * @return true if the symbol exists, false otherwise
     */
    bool has_symbol(const std::string& symbol) const;

    /**
     * @brief Check if an ID exists in the lookup table
     * @param id The ID to check
     * @return true if the ID exists, false otherwise
     */
    bool has_id(symbol_id_t id) const;

    /**
     * @brief Get the number of symbols in the lookup table
     * @return The number of symbols
     */
    size_t size() const;

    /**
     * @brief Get all symbols in the lookup table
     * @return Vector of all symbol strings
     */
    std::vector<std::string> get_all_symbols() const;

   private:
    /**
     * @brief Default constructor - private to enforce singleton pattern
     */
    SymbolLookup() = default;

    /**
     * @brief Constructor that initializes the lookup table with a list of
     * symbols - private to enforce singleton pattern
     * @param symbols Vector of symbol strings to initialize with
     */
    explicit SymbolLookup(const std::vector<std::string>& symbols);

    std::unordered_map<std::string, symbol_id_t> symbol_to_id_;
    std::unordered_map<symbol_id_t, std::string> id_to_symbol_;
    symbol_id_t next_id_ =
        1;  // Start from 1, 0 is reserved for INVALID_SYMBOL_ID
};

}  // namespace utils
}  // namespace thales
