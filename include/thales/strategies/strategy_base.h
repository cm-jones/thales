#ifndef THALES_STRATEGIES_STRATEGY_BASE_H
#define THALES_STRATEGIES_STRATEGY_BASE_H

#include <memory>
#include <string>
#include <vector>
#include <thales/utils/config.h>
#include <thales/data/market_data.h>
#include <thales/core/portfolio.h>

namespace thales {
namespace strategies {

/**
 * @struct Signal
 * @brief Represents a trading signal generated by a strategy.
 */
struct Signal {
    enum class Type {
        BUY,
        SELL,
        HOLD
    };
    
    std::string symbol;       // Symbol of the instrument
    Type type;                // Signal type
    double strength;          // Signal strength (0.0 to 1.0)
    double target_price;      // Target price
    double stop_loss;         // Stop loss price
    std::string timestamp;    // Timestamp when the signal was generated
    std::string strategy_name; // Name of the strategy that generated the signal
    
    // Constructor
    Signal(
        const std::string& sym = "",
        Type t = Type::HOLD,
        double str = 0.0,
        double target = 0.0,
        double stop = 0.0,
        const std::string& ts = "",
        const std::string& strat = ""
    ) : symbol(sym),
        type(t),
        strength(str),
        target_price(target),
        stop_loss(stop),
        timestamp(ts),
        strategy_name(strat) {}
};

/**
 * @class StrategyBase
 * @brief Base class for trading strategies.
 * 
 * This class defines the interface for trading strategies and provides
 * common functionality.
 */
class StrategyBase {
public:
    /**
     * @brief Constructor
     * @param name The name of the strategy
     * @param config The configuration for the strategy
     */
    StrategyBase(const std::string& name, const utils::Config& config);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~StrategyBase() = default;
    
    /**
     * @brief Initialize the strategy
     * @return true if initialization was successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Execute the strategy
     * @param market_data The market data to use for execution
     * @param portfolio The current portfolio
     * @return A vector of signals generated by the strategy
     */
    virtual std::vector<Signal> execute(
        const std::vector<data::MarketData>& market_data,
        const core::Portfolio& portfolio
    ) = 0;
    
    /**
     * @brief Get the name of the strategy
     * @return The name of the strategy
     */
    std::string get_name() const;
    
    /**
     * @brief Get the description of the strategy
     * @return The description of the strategy
     */
    std::string get_description() const;
    
    /**
     * @brief Set the description of the strategy
     * @param description The description to set
     */
    void set_description(const std::string& description);
    
    /**
     * @brief Get the configuration of the strategy
     * @return The configuration of the strategy
     */
    const utils::Config& get_config() const;
    
    /**
     * @brief Set a configuration value
     * @param key The configuration key
     * @param value The value to set
     */
    void set_config_value(const std::string& key, const utils::Config::ConfigValue& value);
    
    /**
     * @brief Get the symbols that the strategy is interested in
     * @return A vector of symbols
     */
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

} // namespace strategies
} // namespace thales

#endif // THALES_STRATEGIES_STRATEGY_BASE_H
