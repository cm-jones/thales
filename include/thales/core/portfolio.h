#ifndef THALES_CORE_PORTFOLIO_H
#define THALES_CORE_PORTFOLIO_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <thales/utils/config.h>

namespace thales {
namespace core {

/**
 * @struct Position
 * @brief Represents a position in a financial instrument.
 */
struct Position {
    std::string symbol;       // Symbol of the instrument
    double quantity;          // Quantity held
    double averagePrice;      // Average entry price
    double currentPrice;      // Current market price
    double unrealizedPnL;     // Unrealized profit/loss
    double realizedPnL;       // Realized profit/loss
    
    // Constructor
    Position(
        const std::string& sym = "",
        double qty = 0.0,
        double avgPrice = 0.0,
        double currPrice = 0.0,
        double unrealPnL = 0.0,
        double realPnL = 0.0
    ) : symbol(sym),
        quantity(qty),
        averagePrice(avgPrice),
        currentPrice(currPrice),
        unrealizedPnL(unrealPnL),
        realizedPnL(realPnL) {}
};

/**
 * @struct Order
 * @brief Represents an order to buy or sell a financial instrument.
 */
struct Order {
    enum class Type {
        MARKET,
        LIMIT,
        STOP,
        STOP_LIMIT
    };
    
    enum class Side {
        BUY,
        SELL
    };
    
    enum class Status {
        PENDING,
        FILLED,
        PARTIALLY_FILLED,
        CANCELED,
        REJECTED
    };
    
    std::string orderId;      // Unique order ID
    std::string symbol;       // Symbol of the instrument
    Type type;                // Order type
    Side side;                // Buy or sell
    double quantity;          // Quantity to trade
    double price;             // Limit price (if applicable)
    double stopPrice;         // Stop price (if applicable)
    Status status;            // Order status
    double filledQuantity;    // Quantity that has been filled
    double averageFillPrice;  // Average fill price
    std::string timestamp;    // Timestamp when the order was created
    
    // Constructor
    Order(
        const std::string& id = "",
        const std::string& sym = "",
        Type t = Type::MARKET,
        Side s = Side::BUY,
        double qty = 0.0,
        double p = 0.0,
        double stopP = 0.0,
        Status stat = Status::PENDING,
        double filledQty = 0.0,
        double avgFillPrice = 0.0,
        const std::string& ts = ""
    ) : orderId(id),
        symbol(sym),
        type(t),
        side(s),
        quantity(qty),
        price(p),
        stopPrice(stopP),
        status(stat),
        filledQuantity(filledQty),
        averageFillPrice(avgFillPrice),
        timestamp(ts) {}
};

/**
 * @class Portfolio
 * @brief Manages the portfolio of positions and orders.
 * 
 * This class is responsible for tracking positions, orders, and
 * calculating portfolio metrics such as value, profit/loss, etc.
 */
class Portfolio {
public:
    /**
     * @brief Constructor
     * @param config The configuration for the portfolio
     */
    explicit Portfolio(const utils::Config& config);
    
    /**
     * @brief Destructor
     */
    ~Portfolio();
    
    /**
     * @brief Initialize the portfolio
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Get all positions in the portfolio
     * @return A vector of positions
     */
    std::vector<Position> getPositions() const;
    
    /**
     * @brief Get a specific position by symbol
     * @param symbol The symbol to look up
     * @return The position, or an empty position if not found
     */
    Position getPosition(const std::string& symbol) const;
    
    /**
     * @brief Get all open orders
     * @return A vector of open orders
     */
    std::vector<Order> getOpenOrders() const;
    
    /**
     * @brief Get all orders for a specific symbol
     * @param symbol The symbol to look up
     * @return A vector of orders for the symbol
     */
    std::vector<Order> getOrders(const std::string& symbol) const;
    
    /**
     * @brief Get the total portfolio value
     * @return The total value of all positions
     */
    double getTotalValue() const;
    
    /**
     * @brief Get the total unrealized profit/loss
     * @return The total unrealized profit/loss
     */
    double getTotalUnrealizedPnL() const;
    
    /**
     * @brief Get the total realized profit/loss
     * @return The total realized profit/loss
     */
    double getTotalRealizedPnL() const;
    
    /**
     * @brief Update a position with new market data
     * @param symbol The symbol to update
     * @param currentPrice The current market price
     */
    void updatePosition(const std::string& symbol, double currentPrice);
    
    /**
     * @brief Add a new position to the portfolio
     * @param position The position to add
     */
    void addPosition(const Position& position);
    
    /**
     * @brief Update an order status
     * @param orderId The order ID to update
     * @param status The new status
     * @param filledQuantity The filled quantity
     * @param averageFillPrice The average fill price
     */
    void updateOrder(
        const std::string& orderId,
        Order::Status status,
        double filledQuantity = 0.0,
        double averageFillPrice = 0.0
    );
    
    /**
     * @brief Add a new order to the portfolio
     * @param order The order to add
     */
    void addOrder(const Order& order);
    
    /**
     * @brief Cancel an order
     * @param orderId The order ID to cancel
     * @return true if the order was canceled, false otherwise
     */
    bool cancelOrder(const std::string& orderId);

private:
    // Configuration
    utils::Config config_;
    
    // Positions and orders
    std::map<std::string, Position> positions_;
    std::map<std::string, Order> orders_;
    
    // Mutex for thread safety
    mutable std::mutex mutex_;
    
    // Private methods
    void updatePositionFromOrder(const Order& order);
};

} // namespace core
} // namespace thales

#endif // THALES_CORE_PORTFOLIO_H
