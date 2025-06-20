// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "thales/core/portfolio.hpp"
#include "thales/data/market_data.hpp"
#include "thales/utils/config.hpp"

namespace thales {
namespace strategies {

/// @struct Signal
/// @brief Represents a trading signal generated by a strategy.
struct CACHE_ALIGNED Signal {
    enum struct Type : uint8_t {
        BUY,
        SELL,
        HOLD,
    } type;  // 1 byte

    std::string symbol;         // 24 bytes on 64-bit systems
    std::string timestamp;      // 24 bytes
    std::string strategy_name;  // 24 bytes
    double strength;            // 8 bytes
    double target_price;        // 8 bytes
    double stop_loss;           // 8 bytes
    // Total: 97 bytes, will be aligned to 128 bytes for cache line alignment

    // Parameter struct for Signal constructor to avoid parameter confusion
    struct SignalParams {
        std::string symbol = "";
        Type type = Type::HOLD;
        double strength = 0.0;
        double target_price = 0.0;
        double stop_loss = 0.0;
        std::string timestamp = "";
        std::string strategy_name = "";
    };

    // Default constructor
    Signal()
        : type(Type::HOLD),
          symbol(""),
          timestamp(""),
          strategy_name(""),
          strength(0.0),
          target_price(0.0),
          stop_loss(0.0) {}

    // Constructor using parameter struct
    explicit Signal(const SignalParams &params)
        : type(params.type),
          symbol(params.symbol),
          timestamp(params.timestamp),
          strategy_name(params.strategy_name),
          strength(params.strength),
          target_price(params.target_price),
          stop_loss(params.stop_loss) {}
};

/// @class StrategyBase
/// @brief Base class for trading strategies.
/// @note This class defines the interface for trading strategies and provides
/// common functionality.
class StrategyBase {
   public:
    /// @brief Constructor
    /// @param name The name of the strategy
    /// @param config The configuration for the strategy
    StrategyBase(const std::string &name, const utils::Config &config);

    /// @brief Virtual destructor
    virtual ~StrategyBase() = default;

    /// @brief Initialize the strategy
    /// @return true if initialization was successful, false otherwise
    virtual bool initialize() = 0;

    /// @brief Execute the strategy
    /// @param market_data The market data to use for execution
    /// @param portfolio The current portfolio
    /// @return A vector of signals generated by the strategy
    virtual std::vector<Signal> execute(
        const std::vector<data::MarketData> &market_data,
        const core::Portfolio &portfolio) = 0;

    /// @brief Get the name of the strategy
    /// @return The name of the strategy
    std::string get_name() const;

    /// @brief Get the description of the strategy
    /// @return The description of the strategy
    std::string get_description() const;

    /// @brief Set the description of the strategy
    /// @param description The description to set
    void set_description(const std::string &description);

    /// @brief Get the configuration of the strategy
    /// @return The configuration of the strategy
    const utils::Config &get_config() const;

    /// @brief Set a configuration value
    /// @param key The configuration key
    /// @param value The value to set
    void set_config_value(const std::string &key,
                          const utils::Config::ConfigValue &value);

    /// @brief Get the symbols that the strategy is interested in
    /// @return A vector of symbols
    virtual std::vector<std::string> get_symbols() const = 0;

   protected:
    // Strategy name and description
    std::string name_;
    std::string description_;

    // Configuration
    utils::Config config_;

    // Symbols to trade
    std::vector<std::string> symbols_;

    // Protected methods
    virtual void validate_config();
    virtual void load_symbols();
};

}  // namespace strategies
}  // namespace thales
