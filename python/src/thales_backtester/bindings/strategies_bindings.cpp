#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <thales/strategies/strategy_base.hpp>
#include <thales/strategies/black_scholes_arbitrage.hpp>
#include <thales/utils/config.hpp>
#include <thales/core/portfolio.hpp>
#include <thales/data/market_data.hpp>

namespace py = pybind11;

void bind_strategies(py::module& m) {
    // Bind Signal struct
    py::enum_<thales::strategies::Signal::Type>(m, "SignalType")
        .value("BUY", thales::strategies::Signal::Type::BUY)
        .value("SELL", thales::strategies::Signal::Type::SELL)
        .value("HOLD", thales::strategies::Signal::Type::HOLD)
        .export_values();
        
    py::class_<thales::strategies::Signal>(m, "Signal")
        .def(py::init<const std::string&, 
                    thales::strategies::Signal::Type,
                    double, double, double,
                    const std::string&, const std::string&>(),
             py::arg("symbol") = "", 
             py::arg("type") = thales::strategies::Signal::Type::HOLD,
             py::arg("strength") = 0.0, 
             py::arg("target_price") = 0.0,
             py::arg("stop_loss") = 0.0, 
             py::arg("timestamp") = "",
             py::arg("strategy_name") = "")
        .def_readwrite("symbol", &thales::strategies::Signal::symbol)
        .def_readwrite("strength", &thales::strategies::Signal::strength)
        .def_readwrite("target_price", &thales::strategies::Signal::target_price)
        .def_readwrite("stop_loss", &thales::strategies::Signal::stop_loss)
        .def_readwrite("timestamp", &thales::strategies::Signal::timestamp)
        .def_readwrite("strategy_name", &thales::strategies::Signal::strategy_name)
        .def_readwrite("type", &thales::strategies::Signal::type);

    // Bind Config value
    py::class_<thales::utils::Config::ConfigValue>(m, "ConfigValue")
        .def(py::init<>())
        .def(py::init<bool>())
        .def(py::init<int>())
        .def(py::init<double>())
        .def(py::init<std::string>());

    // Bind Config
    py::class_<thales::utils::Config>(m, "Config")
        .def(py::init<>())
        .def("get_bool", &thales::utils::Config::get_bool)
        .def("get_int", &thales::utils::Config::get_int)
        .def("get_double", &thales::utils::Config::get_double)
        .def("get_string", &thales::utils::Config::get_string)
        .def("set", &thales::utils::Config::set<bool>)
        .def("set", &thales::utils::Config::set<int>)
        .def("set", &thales::utils::Config::set<double>)
        .def("set", &thales::utils::Config::set<std::string>);

    // Bind MarketData
    py::class_<thales::data::MarketData>(m, "MarketData")
        .def(py::init<>())
        .def_readwrite("symbol", &thales::data::MarketData::symbol)
        .def_readwrite("timestamp", &thales::data::MarketData::timestamp)
        .def_readwrite("price", &thales::data::MarketData::price)
        .def_readwrite("volume", &thales::data::MarketData::volume);

    // Bind Portfolio
    py::class_<thales::core::Portfolio>(m, "Portfolio")
        .def(py::init<>());

    // Bind StrategyBase (abstract class)
    py::class_<thales::strategies::StrategyBase>(m, "StrategyBase")
        .def("get_name", &thales::strategies::StrategyBase::get_name)
        .def("get_description", &thales::strategies::StrategyBase::get_description)
        .def("set_description", &thales::strategies::StrategyBase::set_description)
        .def("get_config", &thales::strategies::StrategyBase::get_config, py::return_value_policy::reference)
        .def("get_symbols", &thales::strategies::StrategyBase::get_symbols);

    // Bind BlackScholesArbitrage
    py::class_<thales::strategies::BlackScholesArbitrage, thales::strategies::StrategyBase>(m, "BlackScholesArbitrage")
        .def(py::init<const thales::utils::Config&>())
        .def("initialize", &thales::strategies::BlackScholesArbitrage::initialize)
        .def("execute", &thales::strategies::BlackScholesArbitrage::execute);
}
