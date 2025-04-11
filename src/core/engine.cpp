#include <chrono>
#include <thales/core/engine.hpp>
#include <thales/utils/logger.hpp>
#include <thread>


#if defined(__linux__)
    #include <sched.h>
    #include <pthread.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__APPLE__) || defined(__MACH__)
    #include <mach/thread_policy.h>
    #include <mach/thread_act.h>
#endif

namespace thales {
namespace core {

Engine::Engine(const utils::Config& config)
    : config_(config), running_(false) {}

Engine::~Engine() { stop(); }

bool Engine::initialize() {
    auto& logger = utils::Logger::get_instance();
    logger.info("Initializing trading engine...");

    // Initialize data manager
    logger.info("Initializing data manager...");
    data_manager_ = std::make_unique<data::DataManager>(config_);
    if (!data_manager_->initialize()) {
        logger.error("Failed to initialize data manager");
        return false;
    }

    // Initialize portfolio
    logger.info("Initializing portfolio...");
    portfolio_ = std::make_unique<Portfolio>(config_);
    if (!portfolio_->initialize()) {
        logger.error("Failed to initialize portfolio");
        return false;
    }

    // Initialize strategy registry
    logger.info("Initializing strategy registry...");
    strategy_registry_ =
        std::make_unique<strategies::StrategyRegistry>(config_);
    if (!strategy_registry_->initialize()) {
        logger.error("Failed to initialize strategy registry");
        return false;
    }

    logger.info("Trading engine initialized successfully");
    return true;
}

void Engine::run() {
    auto& logger = utils::Logger::get_instance();
    logger.info("Starting trading engine...");

    running_ = true;
    
    main_loop();

    running_ = false;
    logger.info("Trading engine stopped");
}

void Engine::stop() {
    if (running_) {
        auto& logger = utils::Logger::get_instance();
        logger.info("Stopping trading engine...");
        running_ = false;
    }
}

bool Engine::is_running() const { return running_; }

void Engine::main_loop() {
    auto& logger = utils::Logger::get_instance();
    
    // Get the loop interval from configuration
    int loop_interval_ms = config_.get_int("engine.loopIntervalMs", 1000);

#if defined(__linux__)
    int current_core = sched_getcpu();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(current_core, &cpuset);
    
    int result = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (result != 0) {
        logger.warning("Failed to set thread affinity for main loop");
    }

    logger.info("Main loop thread pinned to core: " + std::to_string(current_core));
#elif defined(_WIN32) || defined(_WIN64)
    HANDLE thread = GetCurrentThread();
    DWORD_PTR current_mask = SetThreadAffinityMask(thread, GetThreadAffinityMask(thread));
    if (current_mask != 0) {
        logger.info("Main loop thread affinity preserved on Windows");
    } else {
        logger.warning("Failed to set thread affinity on Windows: " + std::to_string(GetLastError()));
    }
#elif defined(__APPLE__) || defined(__MACH__)
    // macOS thread affinity is very limited
    logger.info("Thread pinning not fully supported on macOS");
    // macOS doesn't provide API for thread-to-core pinning in user space
    // You could use thread_policy_set() but it's not reliable for strict pinning
#else
    logger.info("Thread pinning not implemented for this platform");
#endif

    logger.info("Main loop started with interval: " + std::to_string(loop_interval_ms) + "ms");

    while (running_) {
        // Process signals from strategies
        process_signals();

        // Execute orders
        execute_orders();

        // Update portfolio
        update_portfolio();

        // Sleep for the configured interval
        std::this_thread::sleep_for(
            std::chrono::milliseconds(loop_interval_ms));
    }
}

void Engine::process_signals() {
    // Execute strategies to generate signals
    if (strategy_registry_) {
        strategy_registry_->execute_strategies();
    }
}

void Engine::execute_orders() {
    if (!portfolio_) return;

    auto& logger = utils::Logger::get_instance();
    logger.debug("Executing orders...");

    // Get open orders
    boost::container::static_vector<Order, 256> open_orders = portfolio_->get_open_orders();

    if (open_orders.empty()) {
        logger.debug("No orders to execute");
        return;
    }

    logger.info("Executing " + std::to_string(open_orders.size()) + " orders");

    // Execute orders through the IB client
    for (const auto& order : open_orders) {
        // In real implementation, this would check for fill status, etc.
        std::string symbol = utils::SymbolLookup::get_instance().get_symbol(order.symbol_id);
        logger.info(
            "Processing order: " + symbol +
            ", " + order.side_to_string() + ", " + order.type_to_string() + 
            (order.type != Order::Type::MARKET ? ", Price: " + std::to_string(order.price.limit) : ""));
    }
}

void Engine::update_portfolio() {
    if (!portfolio_ || !data_manager_) return;

    auto& symbol_lookup = utils::SymbolLookup::get_instance();

    auto& logger = utils::Logger::get_instance();
    logger.debug("Updating portfolio...");

    // Get positions
    auto positions = portfolio_->get_positions();

    // Update each position with latest market data
    for (const auto& position : positions) {
        std::string symbol = symbol_lookup.get_symbol(position.option.symbol_id);
        auto market_data =
            data_manager_->get_latest_market_data(symbol);
        portfolio_->update_position(symbol, market_data.price);
    }

    // Log portfolio value
    logger.info("Portfolio value: $" +
                std::to_string(portfolio_->get_total_value()));
    logger.info("Unrealized P&L: $" +
                std::to_string(portfolio_->get_total_unrealized_pnl()));
    logger.info("Realized P&L: $" +
                std::to_string(portfolio_->get_total_realized_pnl()));
}

}  // namespace core
}  // namespace thales
