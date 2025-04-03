#pragma once

#include <string>
#include <vector>

namespace thales {
namespace data {

/**
 * @struct MarketData
 * @brief Represents market data for a financial instrument.
 */
struct MarketData {
    virtual ~MarketData() = default;
    std::string symbol;     // Symbol of the instrument
    std::string timestamp;  // Timestamp of the data
    double price;           // Last trade price
    double open;            // Opening price
    double high;            // Highest price
    double low;             // Lowest price
    double close;           // Closing price
    double volume;          // Trading volume
    double bid;             // Best bid price
    double ask;             // Best ask price
    double bidSize;         // Best bid size
    double askSize;         // Best ask size

    // Constructor
    MarketData(const std::string& sym = "", const std::string& ts = "",
               double p = 0.0, double o = 0.0, double h = 0.0, double l = 0.0,
               double c = 0.0, double v = 0.0, double b = 0.0, double a = 0.0,
               double bs = 0.0, double as = 0.0)
        : symbol(sym),
          timestamp(ts),
          price(p),
          open(o),
          high(h),
          low(l),
          close(c),
          volume(v),
          bid(b),
          ask(a),
          bidSize(bs),
          askSize(as) {}
};

/**
 * @struct OptionData
 * @brief Represents market data for an options contract.
 */
struct OptionData : public MarketData {
    virtual ~OptionData() override = default;
    std::string underlyingSymbol;  // Symbol of the underlying asset
    std::string expirationDate;    // Expiration date of the option
    double strikePrice;            // Strike price of the option
    bool isCall;  // Whether the option is a call (true) or put (false)
    double impliedVolatility;  // Implied volatility
    double delta;              // Delta greek
    double gamma;              // Gamma greek
    double theta;              // Theta greek
    double vega;               // Vega greek
    double rho;                // Rho greek
    double openInterest;       // Open interest

    // Constructor
    OptionData(const std::string& sym = "", const std::string& ts = "",
               double p = 0.0, double o = 0.0, double h = 0.0, double l = 0.0,
               double c = 0.0, double v = 0.0, double b = 0.0, double a = 0.0,
               double bs = 0.0, double as = 0.0,
               const std::string& underlying = "",
               const std::string& expDate = "", double strike = 0.0,
               bool call = true, double iv = 0.0, double d = 0.0,
               double g = 0.0, double t = 0.0, double vg = 0.0, double r = 0.0,
               double oi = 0.0)
        : MarketData(sym, ts, p, o, h, l, c, v, b, a, bs, as),
          underlyingSymbol(underlying),
          expirationDate(expDate),
          strikePrice(strike),
          isCall(call),
          impliedVolatility(iv),
          delta(d),
          gamma(g),
          theta(t),
          vega(vg),
          rho(r),
          openInterest(oi) {}
};

/**
 * @struct Bar
 * @brief Represents a time-based bar (OHLCV) for a financial instrument.
 */
struct Bar {
    std::string symbol;     // Symbol of the instrument
    std::string timestamp;  // Timestamp of the bar
    std::string interval;   // Bar interval (e.g., "1min", "1hour", "1day")
    double open;            // Opening price
    double high;            // Highest price
    double low;             // Lowest price
    double close;           // Closing price
    double volume;          // Trading volume

    // Constructor
    Bar(const std::string& sym = "", const std::string& ts = "",
        const std::string& intv = "", double o = 0.0, double h = 0.0,
        double l = 0.0, double c = 0.0, double v = 0.0)
        : symbol(sym),
          timestamp(ts),
          interval(intv),
          open(o),
          high(h),
          low(l),
          close(c),
          volume(v) {}
};

}  // namespace data
}  // namespace thales
