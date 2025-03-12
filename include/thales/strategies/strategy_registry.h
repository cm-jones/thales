#ifndef THALES_STRATEGIES_STRATEGY_REGISTRY_H
#define THALES_STRATEGIES_STRATEGY_REGISTRY_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <thales/utils/config.h>
#include <thales/strategies/strategy_base.h>

namespace thales {
namespace strategies {

/**
 * @class StrategyRegistry
 * @brief Registry for trading strategies.
 * 
 * This class manages the registration, initialization, and execution
 * of trading strategies.
 */
class StrategyRegistry {
public:
    /**
     * @brief Constructor
     * @param config The configuration for the strategy registry
     */
    explicit StrategyRegistry(const utils::Config& config);
    
    /**
     * @brief Destructor
     */
    ~StrategyRegistry();
    
    /**
     * @brief Initialize the strategy registry
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Register a strategy
     * @param strategy The strategy to register
     * @return true if registration was successful, false otherwise
     */
    bool registerStrategy(std::unique_ptr<StrategyBase> strategy);
    
    /**
     * @brief Get a strategy by name
     * @param name The name of the strategy to get
     * @return A pointer to the strategy, or nullptr if not found
     */
    StrategyBase* getStrategy(const std::string& name) const;
    
    /**
     * @brief Get all registered strategies
     * @return A vector of pointers to all registered strategies
     */
    std::vector<StrategyBase*> getAllStrategies() const;
    
    /**
     * @brief Execute all registered strategies
     * @return true if execution was successful, false otherwise
     */
    bool executeStrategies();
    
    /**
     * @brief Enable a strategy
     * @param name The name of the strategy to enable
     * @return true if the strategy was enabled, false otherwise
     */
    bool enableStrategy(const std::string& name);
    
    /**
     * @brief Disable a strategy
     * @param name The name of the strategy to disable
     * @return true if the strategy was disabled, false otherwise
     */
    bool disableStrategy(const std::string& name);
    
    /**
     * @brief Check if a strategy is enabled
     * @param name The name of the strategy to check
     * @return true if the strategy is enabled, false otherwise
     */
    bool isStrategyEnabled(const std::string& name) const;

private:
    // Configuration
    utils::Config config_;
    
    // Registered strategies
    std::unordered_map<std::string, std::unique_ptr<StrategyBase>> strategies_;
    
    // Enabled strategies
    std::unordered_map<std::string, bool> enabledStrategies_;
    
    // Private methods
    void loadStrategiesFromConfig();
};

} // namespace strategies
} // namespace thales

#endif // THALES_STRATEGIES_STRATEGY_REGISTRY_H
