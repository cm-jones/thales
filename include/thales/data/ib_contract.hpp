// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace thales {
namespace data {

/**
 * @class IBContract
 * @brief Represents a financial instrument contract in the Interactive Brokers
 * API.
 *
 * This class is a simplified version of the IB::Contract class from the IB API.
 * It contains the essential fields needed to identify and describe a financial
 * instrument for trading purposes.
 */
class IBContract {
   public:
    /**
     * @brief Default constructor
     */
    IBContract() = default;

    /**
     * @brief Constructor with essential parameters
     * @param symbol The symbol of the contract
     * @param sec_type The security type (STK, OPT, FUT, etc.)
     * @param exchange The exchange where the contract is traded
     * @param currency The currency of the contract
     */
    IBContract(const std::string &symbol, const std::string &sec_type,
               const std::string &exchange, const std::string &currency);

    /**
     * @brief Destructor
     */
    ~IBContract() = default;

    // Contract identification
    int contract_id = 0;           // Contract ID
    std::string symbol;            // Symbol
    std::string sec_type;          // Security type (STK, OPT, FUT, etc.)
    std::string last_trade_date;   // Last trade date (YYYYMMDD format)
    double strike = 0.0;           // Strike price for options
    std::string right;             // Option right (C for call, P for put)
    std::string multiplier;        // Contract multiplier
    std::string exchange;          // Exchange
    std::string primary_exchange;  // Primary exchange
    std::string currency;          // Currency
    std::string local_symbol;      // Local symbol
    std::string trading_class;     // Trading class

    // For options on futures or options
    std::string include_expired;  // Include expired contracts

    // For combo contracts
    std::vector<IBContract> combo_legs;  // Combo legs

    // For delta-neutral contracts
    bool delta_neutral = false;  // Delta neutral flag
    std::shared_ptr<IBContract>
        delta_neutral_contract;  // Delta neutral contract (using shared_ptr for
                                 // copyability)
    double delta_neutral_aux_price = 0.0;  // Delta neutral auxiliary price

    /**
     * @brief Convert to a string representation
     * @return String representation of the contract
     */
    std::string to_string() const;
};

/**
 * @class IBContractDetails
 * @brief Represents detailed information about a contract in the Interactive
 * Brokers API.
 *
 * This class is a simplified version of the IB::ContractDetails class from the
 * IB API. It contains the contract itself plus additional details about the
 * contract.
 */
class IBContractDetails {
   public:
    /**
     * @brief Default constructor
     */
    IBContractDetails() = default;

    /**
     * @brief Constructor with contract
     * @param contract The contract
     */
    explicit IBContractDetails(const IBContract &contract);

    /**
     * @brief Destructor
     */
    ~IBContractDetails() = default;

    // The contract
    IBContract contract;

    // Contract details
    std::string market_name;           // Market name
    std::string trading_hours;         // Trading hours
    std::string liquid_hours;          // Liquid hours
    std::string time_zone_id;          // Time zone ID
    std::string category;              // Category
    std::string subcategory;           // Subcategory
    double min_tick = 0.0;             // Minimum tick
    std::string price_magnifier;       // Price magnifier
    int order_types = 0;               // Order types
    int valid_exchanges = 0;           // Valid exchanges
    int underlying_contract_id = 0;    // Underlying contract ID
    std::string long_name;             // Long name
    std::string contract_month;        // Contract month
    std::string industry;              // Industry
    std::string category_industry;     // Category industry
    std::string subcategory_industry;  // Subcategory industry
    std::string time_zone;             // Time zone
    std::string trading_class;         // Trading class
    std::string description;           // Description
    std::string bond_type;             // Bond type
    std::string coupon_type;           // Coupon type
    double coupon = 0.0;               // Coupon
    std::string maturity;              // Maturity
    std::string issue_date;            // Issue date
    std::string ratings;               // Ratings
    std::string bond_issuer;           // Bond issuer
    std::string cusip;                 // CUSIP
    std::string notes;                 // Notes

    /**
     * @brief Convert to a string representation
     * @return String representation of the contract details
     */
    std::string to_string() const;
};

/**
 * @class IBContractDescription
 * @brief Represents a contract description in the Interactive Brokers API.
 *
 * This class is a simplified version of the IB::ContractDescription class from
 * the IB API. It contains a contract and a list of derivative security types.
 */
class IBContractDescription {
   public:
    /**
     * @brief Default constructor
     */
    IBContractDescription() = default;

    /**
     * @brief Constructor with contract
     * @param contract The contract
     */
    explicit IBContractDescription(const IBContract &contract);

    /**
     * @brief Destructor
     */
    ~IBContractDescription() = default;

    // The contract
    IBContract contract;

    // Derivative security types
    std::vector<std::string> derivative_sec_types;

    /**
     * @brief Convert to a string representation
     * @return String representation of the contract description
     */
    std::string to_string() const;
};

}  // namespace data
}  // namespace thales
