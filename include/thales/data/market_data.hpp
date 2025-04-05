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
        std::string symbol;     // Symbol of the instrument (24-32 bytes)
        std::string timestamp;  // Timestamp of the data (24-32 bytes)
        double price;           // Last trade price (8 bytes)
        double open;            // Opening price (8 bytes)
        double high;            // Highest price (8 bytes)
        double low;             // Lowest price (8 bytes)
        double close;           // Closing price (8 bytes)
        double volume;          // Trading volume (8 bytes)
        double bid;             // Best bid price (8 bytes)
        double ask;             // Best ask price (8 bytes)
        double bid_size;        // Best bid size (8 bytes)
        double ask_size;        // Best ask size (8 bytes)

        // Constructor
        MarketData(const std::string& sym = "", const std::string& ts = "",
                   double p = 0.0, double o = 0.0, double h = 0.0,
                   double l = 0.0, double c = 0.0, double v = 0.0,
                   double b = 0.0, double a = 0.0, double bs = 0.0,
                   double as = 0.0)
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
              bid_size(bs),
              ask_size(as) {}
};

/**
 * @struct OptionData
 * @brief Represents market data for an options contract.
 */
struct OptionData : public MarketData {
        virtual ~OptionData() override = default;
        std::string
            underlying_symbol;  // Symbol of the underlying asset (24-32 bytes)
        std::string
            expiration_date;  // Expiration date of the option (24-32 bytes)
        double strike_price;  // Strike price of the option (8 bytes)
        double implied_volatility;  // Implied volatility (8 bytes)
        double delta;               // Delta greek (8 bytes)
        double gamma;               // Gamma greek (8 bytes)
        double theta;               // Theta greek (8 bytes)
        double vega;                // Vega greek (8 bytes)
        double rho;                 // Rho greek (8 bytes)
        double open_interest;       // Open interest (8 bytes)
        bool is_call;  // Whether the option is a call (true) or put (false) (1
                       // byte)

        // Constructor
        OptionData(const std::string& sym = "", const std::string& ts = "",
                   double p = 0.0, double o = 0.0, double h = 0.0,
                   double l = 0.0, double c = 0.0, double v = 0.0,
                   double b = 0.0, double a = 0.0, double bs = 0.0,
                   double as = 0.0, const std::string& underlying = "",
                   const std::string& exp_date = "", double strike = 0.0,
                   bool call = true, double iv = 0.0, double d = 0.0,
                   double g = 0.0, double t = 0.0, double vg = 0.0,
                   double r = 0.0, double oi = 0.0)
            : MarketData(sym, ts, p, o, h, l, c, v, b, a, bs, as),
              underlying_symbol(underlying),
              expiration_date(exp_date),
              strike_price(strike),
              implied_volatility(iv),
              delta(d),
              gamma(g),
              theta(t),
              vega(vg),
              rho(r),
              open_interest(oi),
              is_call(call) {}
};

/**
 * @struct Bar
 * @brief Represents a time-based bar (OHLCV) for a financial instrument.
 */
struct Bar {
        std::string symbol;     // Symbol of the instrument (24-32 bytes)
        std::string timestamp;  // Timestamp of the bar (24-32 bytes)
        std::string interval;   // Bar interval (e.g., "1min", "1hour", "1day")
                                // (24-32 bytes)
        double open;            // Opening price (8 bytes)
        double high;            // Highest price (8 bytes)
        double low;             // Lowest price (8 bytes)
        double close;           // Closing price (8 bytes)
        double volume;          // Trading volume (8 bytes)

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
