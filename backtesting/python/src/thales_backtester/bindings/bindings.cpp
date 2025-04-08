#include <pybind11/pybind11.h>

namespace py = pybind11;

// Forward declarations for our binding functions
void bind_models(py::module& m);
void bind_strategies(py::module& m);

// Main module definition
PYBIND11_MODULE(thales_cpp_bindings, m) {
    m.doc() = "Python bindings for Thales C++ library";
    
    // Create submodules
    py::module models_module = m.def_submodule("models", "Thales option pricing models");
    py::module strategies_module = m.def_submodule("strategies", "Thales trading strategies");
    
    // Bind the models and strategies
    bind_models(models_module);
    bind_strategies(strategies_module);
}
