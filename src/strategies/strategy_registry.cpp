#include <thales/strategies/strategy_registry.h>
#include <thales/utils/logger.h>
#include <thales/strategies/black_scholes_arbitrage.h>

namespace thales {
namespace strategies {

StrategyRegistry::StrategyRegistry(const utils::Config& config)
    : config_(config) {
}

StrategyRegistry::~StrategyRegistry() {
}

bool StrategyRegistry::initialize() {
    auto& logger = utils::Logger::getInstance();
    logger.info("Initializing strategy registry");
    
    try {
        // Load strategies from configuration
        loadStrategiesFromConfig();
        
        logger.info("Strategy registry initialized successfully with " + 
                   std::to_string(strategies_.size()) + " strategies");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception during strategy registry initialization: " + std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception during strategy registry initialization");
        return false;
    }
}

bool StrategyRegistry::registerStrategy(std::unique_ptr<StrategyBase> strategy) {
    auto& logger = utils::Logger::getInstance();
    
    if (!strategy) {
        logger.error("Cannot register null strategy");
        return false;
    }
    
    std::string name = strategy->getName();
    
    if (strategies_.find(name) != strategies_.end()) {
        logger.warning("Strategy with name '" + name + "' already registered");
        return false;
    }
    
    logger.info("Registering strategy: " + name);
    
    // Initialize the strategy
    if (!strategy->initialize()) {
        logger.error("Failed to initialize strategy: " + name);
        return false;
    }
    
    // Add the strategy to the registry
    strategies_[name] = std::move(strategy);
    
    // Enable the strategy by default
    enabledStrategies_[name] = true;
    
    logger.info("Strategy registered successfully: " + name);
    return true;
}

StrategyBase* StrategyRegistry::getStrategy(const std::string& name) const {
    auto it = strategies_.find(name);
    if (it == strategies_.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

std::vector<StrategyBase*> StrategyRegistry::getAllStrategies() const {
    std::vector<StrategyBase*> result;
    result.reserve(strategies_.size());
    
    for (const auto& pair : strategies_) {
        result.push_back(pair.second.get());
    }
    
    return result;
}

bool StrategyRegistry::executeStrategies() {
    auto& logger = utils::Logger::getInstance();
    logger.debug("Executing strategies");
    
    // This is a placeholder for the actual implementation
    // In a real implementation, you would:
    // 1. Get the latest market data
    // 2. Get the current portfolio
    // 3. Execute each enabled strategy
    // 4. Collect and process the signals
    
    logger.debug("Strategies executed successfully");
    return true;
}

bool StrategyRegistry::enableStrategy(const std::string& name) {
    auto& logger = utils::Logger::getInstance();
    
    auto it = strategies_.find(name);
    if (it == strategies_.end()) {
        logger.warning("Cannot enable strategy '" + name + "': not found");
        return false;
    }
    
    enabledStrategies_[name] = true;
    logger.info("Strategy enabled: " + name);
    return true;
}

bool StrategyRegistry::disableStrategy(const std::string& name) {
    auto& logger = utils::Logger::getInstance();
    
    auto it = strategies_.find(name);
    if (it == strategies_.end()) {
        logger.warning("Cannot disable strategy '" + name + "': not found");
        return false;
    }
    
    enabledStrategies_[name] = false;
    logger.info("Strategy disabled: " + name);
    return true;
}

bool StrategyRegistry::isStrategyEnabled(const std::string& name) const {
    auto it = enabledStrategies_.find(name);
    if (it == enabledStrategies_.end()) {
        return false;
    }
    
    return it->second;
}

void StrategyRegistry::loadStrategiesFromConfig() {
    auto& logger = utils::Logger::getInstance();
    
    // Check if there are enabled strategies in the configuration
    if (!config_.hasKey("strategies.enabled")) {
        logger.warning("No enabled strategies found in configuration");
        return;
    }
    
    // This is a placeholder for loading strategies from the configuration
    // In a real implementation, you would use a proper JSON library to parse the array
    
    // For now, we'll just add some default strategies
    std::vector<std::string> enabledStrategies = {"BlackScholesArbitrage"};
    
    logger.info("Loading " + std::to_string(enabledStrategies.size()) + " strategies from configuration");
    
    // Register each enabled strategy
    for (const auto& strategyName : enabledStrategies) {
        if (strategyName == "BlackScholesArbitrage") {
            auto strategy = std::make_unique<BlackScholesArbitrage>(config_);
            registerStrategy(std::move(strategy));
        }
        // Add more strategy types here as they are implemented
    }
}

} // namespace strategies
} // namespace thales
