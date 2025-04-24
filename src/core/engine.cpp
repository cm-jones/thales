// SPDX-License-Identifier: MIT

#include <chrono>
#include <thread>

#if defined(__linux__)
#include <pthread.h>
#include <sched.h>
#endif

#include "thales/core/engine.hpp"
#include "thales/utils/logger.hpp"

namespace thales {
namespace core {

#include <mutex>

namespace {
std::mutex signal_mutex_;
std::mutex order_mutex_;
std::mutex portfolio_mutex_;
}  // namespace

Engine::Engine(const utils::Config &config) : config_(config) {
    // No initialization in constructor - following RAII principles
}

Engine::~Engine() {
    stop();
    // Wait for the engine to fully stop
    while (is_running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool Engine::initialize() {
    auto &logger = utils::Logger::get_instance();
    logger.info("Initializing trading engine...");

    try {
        // Initialize data manager (shared resource)
        logger.info("Initializing data manager...");
        data_manager_ = std::make_shared<data::DataManager>(config_);
        if (!data_manager_->initialize()) {
            logger.error("Failed to initialize data manager");
            return false;
        }

        // Initialize portfolio with data manager
        logger.info("Initializing portfolio...");
        portfolio_ = std::make_unique<Portfolio>(config_);
        if (!portfolio_->initialize()) {
            logger.error("Failed to initialize portfolio");
            return false;
        }

        // Initialize strategy registry with data manager and portfolio
        logger.info("Initializing strategy registry...");
        strategy_registry_ = std::make_unique<strategies::StrategyRegistry>(
            config_, std::move(data_manager_), portfolio_.get());
        // Re-acquire data_manager_ since we moved it
        data_manager_ = strategy_registry_->get_data_manager();
        if (!strategy_registry_->initialize()) {
            logger.error("Failed to initialize strategy registry");
            return false;
        }

        logger.info("Trading engine initialized successfully");
        return true;
    } catch (const std::exception &e) {
        logger.error("Engine initialization failed: " + std::string(e.what()));
        return false;
    }
}

void Engine::run() {
    auto &logger = utils::Logger::get_instance();
    logger.info("Starting trading engine...");

    running_ = true;

    main_loop();

    running_ = false;
    logger.info("Trading engine stopped");
}

void Engine::stop() {
    if (running_) {
        auto &logger = utils::Logger::get_instance();
        logger.info("Stopping trading engine...");
        running_ = false;
    }
}

bool Engine::is_running() const { return running_; }

void Engine::main_loop() {
    auto &logger = utils::Logger::get_instance();

    // Configure main loop timing
    const int loop_interval_ms = config_.get_int("engine.loopIntervalMs", 1000);

#if defined(__linux__)
    int current_core = sched_getcpu();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(current_core, &cpuset);

    int result =
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (result != 0) {
        logger.warning("Failed to set thread affinity for main loop");
    }

    logger.info("Main loop thread pinned to core: " +
                std::to_string(current_core));
#endif

    logger.info("Main loop started with interval: " +
                std::to_string(loop_interval_ms) + "ms");

    while (running_) {
        try {
            // Process signals from strategies
            process_signals();

            // Execute orders
            execute_orders();

            // Update portfolio
            update_portfolio();

            // Wait for next cycle
            std::this_thread::sleep_for(
                std::chrono::milliseconds(loop_interval_ms));
        } catch (const std::exception &e) {
            logger.error("Error in main loop: " + std::string(e.what()));
        }
    }
}

void Engine::process_signals() {
    std::lock_guard<std::mutex> lock(signal_mutex_);

    // Execute strategies to generate signals
    if (strategy_registry_) {
        strategy_registry_->execute_strategies();
    }
}

void Engine::execute_orders() {
    std::lock_guard<std::mutex> lock(order_mutex_);

    if (!portfolio_) {
        return;
    }

    auto &logger = utils::Logger::get_instance();
    logger.debug("Executing orders...");

    try {
        // Get open orders
        boost::container::static_vector<Order, 256> open_orders =
            portfolio_->get_open_orders();

        if (open_orders.empty()) {
            logger.debug("No orders to execute");
            return;
        }

        logger.info("Executing " + std::to_string(open_orders.size()) +
                    " orders");

        // Execute orders through the IB client
        for (const auto &order : open_orders) {
            const std::string symbol =
                utils::SymbolLookup::get_instance().get_symbol(order.symbol_id);
            logger.info("Processing order: " + symbol + ", " +
                        order.side_to_string() + ", " + order.type_to_string() +
                        (order.type != Order::Type::MARKET
                             ? ", Price: " + std::to_string(order.price.limit)
                             : ""));
            // TODO: Implement actual order execution logic
        }
    } catch (const std::exception &e) {
        logger.error("Error executing orders: " + std::string(e.what()));
    }
}

void Engine::update_portfolio() {
    std::lock_guard<std::mutex> lock(portfolio_mutex_);

    if (!portfolio_ || !data_manager_) {
        return;
    }

    auto &symbol_lookup = utils::SymbolLookup::get_instance();
    auto &logger = utils::Logger::get_instance();
    logger.debug("Updating portfolio...");

    try {
        // Get current positions
        const auto positions = portfolio_->get_positions();

        // Update positions with latest market data
        for (const auto &position : positions) {
            const std::string symbol =
                symbol_lookup.get_symbol(position.option.symbol_id);
            const auto market_data =
                data_manager_->get_latest_market_data(symbol);
            portfolio_->update_position(symbol, market_data.price);
        }

        // Log portfolio status
        logger.info("Portfolio value: $" +
                    std::to_string(portfolio_->get_total_value()));
        logger.info("Unrealized P&L: $" +
                    std::to_string(portfolio_->get_total_unrealized_pnl()));
        logger.info("Realized P&L: $" +
                    std::to_string(portfolio_->get_total_realized_pnl()));
    } catch (const std::exception &e) {
        logger.error("Error updating portfolio: " + std::string(e.what()));
    }
}

}  // namespace core
}  // namespace thales
