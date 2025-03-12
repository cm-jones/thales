#include <thales/strategies/strategy_base.h>
#include <thales/utils/logger.h>

namespace thales {
namespace strategies {

StrategyBase::StrategyBase(const std::string& name, const utils::Config& config)
    : name_(name), config_(config) {
}

std::string StrategyBase::getName() const {
    return name_;
}

std::string StrategyBase::getDescription() const {
    return description_;
}

void StrategyBase::setDescription(const std::string& description) {
    description_ = description;
}

const utils::Config& StrategyBase::getConfig() const {
    return config_;
}

void StrategyBase::setConfigValue(const std::string& key, const utils::Config::ConfigValue& value) {
    config_.setValue(key, value);
}

void StrategyBase::validateConfig() {
    // Base implementation does nothing
    // Derived classes can override this to validate their specific configuration
}

void StrategyBase::loadSymbols() {
    auto& logger = utils::Logger::getInstance();
    
    // Try to load symbols from the configuration
    std::string configKey = "data.symbols";
    
    if (config_.hasKey(configKey)) {
        // This is a placeholder for loading symbols from the configuration
        // In a real implementation, you would use a proper JSON library to parse the array
        
        // For now, we'll just add some default symbols
        symbols_ = {"SPY", "QQQ", "AAPL", "MSFT", "AMZN", "GOOGL", "FB", "TSLA"};
        
        logger.info("Loaded " + std::to_string(symbols_.size()) + " symbols for strategy " + name_);
    } else {
        logger.warning("No symbols found in configuration for strategy " + name_);
    }
}

} // namespace strategies
} // namespace thales
