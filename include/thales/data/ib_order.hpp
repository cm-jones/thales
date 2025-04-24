// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace thales {
namespace data {

/**
 * @class IBOrder
 * @brief Represents an order in the Interactive Brokers API.
 *
 * This class is a simplified version of the IB::Order class from the IB API.
 * It contains the essential fields needed to place and manage orders.
 */
class IBOrder {
   public:
    /**
     * @brief Default constructor
     */
    IBOrder() = default;

    /**
     * @brief Constructor with essential parameters
     * @param action The action (BUY or SELL)
     * @param total_quantity The total quantity
     * @param order_type The order type (MKT, LMT, STP, etc.)
     */
    IBOrder(const std::string &action, double total_quantity,
            const std::string &order_type);

    /**
     * @brief Destructor
     */
    ~IBOrder() = default;

    // Order identification
    std::string order_id;   // Order ID
    std::string client_id;  // Client ID
    int perm_id = 0;        // Permanent ID

    // Order parameters
    std::string action;           // Action (BUY or SELL)
    double total_quantity = 0.0;  // Total quantity
    std::string order_type;       // Order type (MKT, LMT, STP, etc.)
    double lmt_price = 0.0;       // Limit price
    double aux_price = 0.0;      // Auxiliary price (stop price for stop orders)
    std::string tif;             // Time in force (DAY, GTC, etc.)
    std::string oca_group;       // OCA group
    int oca_type = 0;            // OCA type
    std::string order_ref;       // Order reference
    bool transmit = true;        // Transmit flag
    int parent_id = 0;           // Parent order ID
    bool block_order = false;    // Block order flag
    bool sweep_to_fill = false;  // Sweep to fill flag
    double display_size = 0.0;   // Display size
    int trigger_method = 0;      // Trigger method
    bool outside_rth = false;    // Outside regular trading hours flag
    bool hidden = false;         // Hidden flag
    std::string good_after_time;  // Good after time
    std::string good_till_date;   // Good till date
    std::string rule80A;          // Rule 80A
    double percent_offset = 0.0;  // Percent offset
    bool override_percentage_constraints =
        false;                      // Override percentage constraints flag
    double trail_stop_price = 0.0;  // Trail stop price
    double trailing_percent = 0.0;  // Trailing percent

    // Financial advisor parameters
    std::string fa_group;       // FA group
    std::string fa_profile;     // FA profile
    std::string fa_method;      // FA method
    std::string fa_percentage;  // FA percentage

    // Institutional parameters
    std::string open_close;           // Open/close flag
    int origin = 0;                   // Origin
    int short_sale_slot = 0;          // Short sale slot
    std::string designated_location;  // Designated location
    int exempt_code = -1;             // Exempt code

    // SMART routing parameters
    double discretionary_amt = 0.0;      // Discretionary amount
    bool e_trade_only = true;            // E-trade only flag
    bool firm_quote_only = true;         // Firm quote only flag
    double nbbo_price_cap = 0.0;         // NBBO price cap
    bool opt_out_smart_routing = false;  // Opt out smart routing flag

    // BOX or VOL orders
    int auction_strategy = 0;      // Auction strategy
    double starting_price = 0.0;   // Starting price
    double stock_ref_price = 0.0;  // Stock reference price
    double delta = 0.0;            // Delta

    // Pegged-to-benchmark orders
    double stock_range_lower = 0.0;  // Stock range lower
    double stock_range_upper = 0.0;  // Stock range upper

    // VOLATILITY orders
    double volatility = 0.0;                  // Volatility
    int volatility_type = 0;                  // Volatility type
    std::string delta_neutral_order_type;     // Delta neutral order type
    double delta_neutral_aux_price = 0.0;     // Delta neutral auxiliary price
    int delta_neutral_con_id = 0;             // Delta neutral contract ID
    std::string delta_neutral_settling_firm;  // Delta neutral settling firm
    std::string
        delta_neutral_clearing_account;  // Delta neutral clearing account
    std::string delta_neutral_clearing_intent;  // Delta neutral clearing intent
    std::string delta_neutral_open_close;       // Delta neutral open/close
    bool delta_neutral_short_sale = false;      // Delta neutral short sale flag
    int delta_neutral_short_sale_slot = 0;      // Delta neutral short sale slot
    std::string
        delta_neutral_designated_location;  // Delta neutral designated location

    // COMBO orders
    double basis_points = 0.0;  // Basis points
    int basis_points_type = 0;  // Basis points type

    // SCALE orders
    int scale_init_level_size = 0;          // Scale initial level size
    int scale_subs_level_size = 0;          // Scale subsequent level size
    double scale_price_increment = 0.0;     // Scale price increment
    double scale_price_adjust_value = 0.0;  // Scale price adjust value
    int scale_price_adjust_interval = 0;    // Scale price adjust interval
    double scale_profit_offset = 0.0;       // Scale profit offset
    bool scale_auto_reset = false;          // Scale auto reset flag
    int scale_init_position = 0;            // Scale initial position
    int scale_init_fill_qty = 0;            // Scale initial fill quantity
    bool scale_random_percent = false;      // Scale random percent flag
    std::string scale_table;                // Scale table

    // HEDGE orders
    std::string hedge_type;   // Hedge type
    std::string hedge_param;  // Hedge parameter

    // Clearing parameters
    std::string account;           // Account
    std::string settling_firm;     // Settling firm
    std::string clearing_account;  // Clearing account
    std::string clearing_intent;   // Clearing intent

    // ALGO orders
    std::string algo_strategy;  // Algo strategy

    // What-if
    bool what_if = false;  // What-if flag

    /**
     * @brief Convert to a string representation
     * @return String representation of the order
     */
    std::string to_string() const;
};

/**
 * @class IBOrderState
 * @brief Represents the state of an order in the Interactive Brokers API.
 *
 * This class is a simplified version of the IB::OrderState class from the IB
 * API. It contains information about the current state of an order.
 */
class IBOrderState {
   public:
    /**
     * @brief Default constructor
     */
    IBOrderState() = default;

    /**
     * @brief Constructor with status
     * @param status The order status
     */
    explicit IBOrderState(const std::string &status);

    /**
     * @brief Destructor
     */
    ~IBOrderState() = default;

    // Order state
    std::string status;               // Order status
    std::string init_margin;          // Initial margin
    std::string maint_margin;         // Maintenance margin
    std::string equity_with_loan;     // Equity with loan
    double commission = 0.0;          // Commission
    double min_commission = 0.0;      // Minimum commission
    double max_commission = 0.0;      // Maximum commission
    std::string commission_currency;  // Commission currency
    std::string warning_text;         // Warning text

    /**
     * @brief Convert to a string representation
     * @return String representation of the order state
     */
    std::string to_string() const;
};

/**
 * @class IBExecution
 * @brief Represents an execution in the Interactive Brokers API.
 *
 * This class is a simplified version of the IB::Execution class from the IB
 * API. It contains information about an order execution.
 */
class IBExecution {
   public:
    /**
     * @brief Default constructor
     */
    IBExecution() = default;

    /**
     * @brief Constructor with essential parameters
     * @param exec_id The execution ID
     * @param order_id The order ID
     * @param shares The number of shares executed
     * @param price The execution price
     */
    IBExecution(const std::string &exec_id, const std::string &order_id,
                double shares, double price);

    /**
     * @brief Destructor
     */
    ~IBExecution() = default;

    // Execution identification
    std::string exec_id;   // Execution ID
    std::string order_id;  // Order ID
    int client_id = 0;     // Client ID
    int perm_id = 0;       // Permanent ID

    // Execution details
    std::string time;            // Execution time
    std::string acct_number;     // Account number
    std::string exchange;        // Exchange
    std::string side;            // Side (BOT or SLD)
    double shares = 0.0;         // Shares executed
    double price = 0.0;          // Execution price
    int liquidation = 0;         // Liquidation
    double cum_qty = 0.0;        // Cumulative quantity
    double avg_price = 0.0;      // Average price
    std::string order_ref;       // Order reference
    std::string ev_rule;         // EV rule
    double ev_multiplier = 0.0;  // EV multiplier
    std::string model_code;      // Model code

    /**
     * @brief Convert to a string representation
     * @return String representation of the execution
     */
    std::string to_string() const;
};

}  // namespace data
}  // namespace thales
