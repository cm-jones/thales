// SPDX-License-Identifier: MIT

#include "thales/strategies/strategy_registry.hpp"

#include "thales/strategies/black_scholes_arbitrage.hpp"
#include "thales/utils/logger.hpp"

namespace thales {
namespace strategies {

StrategyRegistry::StrategyRegistry(
    const utils::Config &config,
    std::shared_ptr<data::DataManager> &&data_manager,
    core::Portfolio *portfolio)
    : config_(config),
      data_manager_(std::move(data_manager)),
      portfolio_(portfolio) {}

bool StrategyRegistry::initialize() {
    auto &logger = utils::Logger::get_instance();
    logger.info("Initializing strategy registry");

    try {
        // Load strategies from configuration
        load_strategies_from_config();

        logger.info("Strategy registry initialized successfully with " +
                    std::to_string(strategies_.size()) + " strategies");
        return true;
    } catch (const std::exception &e) {
        logger.error("Exception during strategy registry initialization: " +
                     std::string(e.what()));
        return false;
    } catch (...) {
        logger.error(
            "Unknown exception during strategy registry initialization");
        return false;
    }
}

bool StrategyRegistry::register_strategy(
    std::unique_ptr<StrategyBase> strategy) {
    auto &logger = utils::Logger::get_instance();

    if (!strategy) {
        logger.error("Cannot register null strategy");
        return false;
    }

    std::string name = strategy->get_name();

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
    enabled_strategies_[name] = true;

    logger.info("Strategy registered successfully: " + name);
    return true;
}

StrategyBase *StrategyRegistry::get_strategy(const std::string &name) const {
    auto it = strategies_.find(name);
    if (it == strategies_.end()) {
        return nullptr;
    }

    return it->second.get();
}

std::vector<StrategyBase *> StrategyRegistry::get_all_strategies() const {
    std::vector<StrategyBase *> result;
    result.reserve(strategies_.size());

    for (const auto &pair : strategies_) {
        result.push_back(pair.second.get());
    }

    return result;
}

bool StrategyRegistry::execute_strategies() {
    auto &logger = utils::Logger::get_instance();
    logger.debug("Executing strategies");

    if (!data_manager_ || !portfolio_) {
        logger.error("Cannot execute strategies: missing components");
        return false;
    }

    try {
        // Get the latest market data for all symbols we're interested in
        std::vector<std::string> symbols;
        for (const auto &pair : strategies_) {
            auto strategy_symbols = pair.second->get_symbols();
            symbols.insert(symbols.end(), strategy_symbols.begin(),
                           strategy_symbols.end());
        }

        // Get latest market data for all symbols
        std::vector<data::MarketData> market_data;
        for (const auto &symbol : symbols) {
            auto data = data_manager_->get_latest_market_data(symbol);
            market_data.push_back(std::move(data));
        }

        // Execute each enabled strategy
        for (const auto &pair : strategies_) {
            if (!enabled_strategies_[pair.first]) {
                continue;
            }

            auto &strategy = pair.second;
            logger.debug("Executing strategy: " + strategy->get_name());

            // Execute strategy with latest data
            auto signals = strategy->execute(market_data, *portfolio_);

            // Process signals (to be implemented)
            for (const auto &signal : signals) {
                logger.info("Signal generated: " + signal.symbol + " " +
                            (signal.type == Signal::Type::BUY    ? "BUY"
                             : signal.type == Signal::Type::SELL ? "SELL"
                                                                 : "HOLD"));
            }
        }

        logger.debug("Strategies executed successfully");
        return true;
    } catch (const std::exception &e) {
        logger.error("Error executing strategies: " + std::string(e.what()));
        return false;
    }
}

bool StrategyRegistry::enable_strategy(const std::string &name) {
    auto &logger = utils::Logger::get_instance();

    auto it = strategies_.find(name);
    if (it == strategies_.end()) {
        logger.warning("Cannot enable strategy '" + name + "': not found");
        return false;
    }

    enabled_strategies_[name] = true;
    logger.info("Strategy enabled: " + name);
    return true;
}

bool StrategyRegistry::disable_strategy(const std::string &name) {
    auto &logger = utils::Logger::get_instance();

    auto it = strategies_.find(name);
    if (it == strategies_.end()) {
        logger.warning("Cannot disable strategy '" + name + "': not found");
        return false;
    }

    enabled_strategies_[name] = false;
    logger.info("Strategy disabled: " + name);
    return true;
}

bool StrategyRegistry::is_strategy_enabled(const std::string &name) const {
    auto it = enabled_strategies_.find(name);
    if (it == enabled_strategies_.end()) {
        return false;
    }

    return it->second;
}

void StrategyRegistry::load_strategies_from_config() {
    auto &logger = utils::Logger::get_instance();

    // Check if there are enabled strategies in the configuration
    if (!config_.has_key("strategies.enabled")) {
        logger.warning("No enabled strategies found in configuration");
        return;
    }

    // This is a placeholder for loading strategies from the configuration
    // In a real implementation, you would use a proper JSON library to parse
    // the array

    // For now, we'll just add some default strategies
    std::vector<std::string> enabled_strategies = {"BlackScholes"};

    logger.info("Loading " + std::to_string(enabled_strategies.size()) +
                " strategies from configuration");

    // Register each enabled strategy
    for (const auto &strategy_name : enabled_strategies) {
        if (strategy_name == "BlackScholes") {
            auto strategy = std::make_unique<BlackScholesArbitrage>(config_);
            register_strategy(std::move(strategy));
        }
        // Add more strategy types here as they are implemented
    }
}

}  // namespace strategies
}  // namespace thales
