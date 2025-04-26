#include <pybind11/pybind11.h>
#include "thales/models/black_scholes_model.hpp"

namespace py = pybind11;

void bind_models(py::module& m) {
    // Bind BlackScholesModel class
    py::class_"thales::models::BlackScholesModel>(m, "BlackScholesModel")
        .def(py::init<>())
        .def_static("call_price", &thales::models::BlackScholesModel::call_price,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the price of a European call option")
        .def_static("put_price", &thales::models::BlackScholesModel::put_price,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the price of a European put option")
        .def_static("call_delta", &thales::models::BlackScholesModel::call_delta,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the delta of a call option")
        .def_static("put_delta", &thales::models::BlackScholesModel::put_delta,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the delta of a put option")
        .def_static("gamma", &thales::models::BlackScholesModel::gamma,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the gamma of an option")
        .def_static("vega", &thales::models::BlackScholesModel::vega,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the vega of an option")
        .def_static("call_theta", &thales::models::BlackScholesModel::call_theta,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the theta of a call option")
        .def_static("put_theta", &thales::models::BlackScholesModel::put_theta,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the theta of a put option")
        .def_static("call_rho", &thales::models::BlackScholesModel::call_rho,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the rho of a call option")
        .def_static("put_rho", &thales::models::BlackScholesModel::put_rho,
                   py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
                   "Calculate the rho of a put option")
        .def_static("call_implied_volatility", &thales::models::BlackScholesModel::call_implied_volatility,
                   py::arg("price"), py::arg("S"), py::arg("K"), py::arg("r"), py::arg("T"),
                   py::arg("epsilon") = 0.0001, py::arg("max_iterations") = 100,
                   "Calculate the implied volatility of a call option")
        .def_static("put_implied_volatility", &thales::models::BlackScholesModel::put_implied_volatility,
                   py::arg("price"), py::arg("S"), py::arg("K"), py::arg("r"), py::arg("T"),
                   py::arg("epsilon") = 0.0001, py::arg("max_iterations") = 100,
                   "Calculate the implied volatility of a put option");
}
