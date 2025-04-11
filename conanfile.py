from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy
import os

class ThalesConan(ConanFile):
    name = "thales"
    version = "0.1.0"
    license = "MIT"
    author = "Cody M. Jones"
    url = "https://github.com/cm-jones/thales"
    description = "Ultra low-latency, multi-threaded, highly configurable, and extensible algorithmic trading bot designed for trading options contracts with built-in risk management"
    topics = ("trading", "options", "algorithmic-trading", "finance", "black-scholes")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_tests": [True, False],
        "with_benchmarks": [True, False],
        "with_database": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_tests": False,
        "with_benchmarks": False,
        "with_database": True,
    }
    
    exports_sources = (
        "CMakeLists.txt", 
        "src/*", 
        "include/*", 
        "tests/*", 
        "benchmarks/*", 
        "third_party/*",
        "scripts/*",
        "config/*",
        "LICENSE",
        "README.md",
        "CHANGELOG.md",
        "CONTRIBUTING.md",
    )
    
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
    
    def requirements(self):
        self.requires("boost/1.80.0")
        self.requires("yaml-cpp/0.7.0")  # YAML parser
        if self.options.with_database:
            self.requires("libpqxx/7.7.0")
        self.requires("gtest/1.12.1", private=True)
    
    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_POSITION_INDEPENDENT_CODE"] = self.options.get_safe("fPIC", True)
        tc.variables["BUILD_TESTING"] = self.options.with_tests
        tc.variables["BUILD_BENCHMARKS"] = self.options.with_benchmarks
        tc.variables["BUILD_WITH_DATABASE"] = self.options.with_database
        tc.generate()
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.with_tests:
            cmake.test()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
        
        # Copy license and documentation
        copy(self, "LICENSE", self.source_folder, os.path.join(self.package_folder, "licenses"))
        copy(self, "README.md", self.source_folder, os.path.join(self.package_folder, "docs"))
        copy(self, "CHANGELOG.md", self.source_folder, os.path.join(self.package_folder, "docs"))
        
        # Copy include files
        copy(self, "*.h", os.path.join(self.source_folder, "include"), os.path.join(self.package_folder, "include"))
        copy(self, "*.hpp", os.path.join(self.source_folder, "include"), os.path.join(self.package_folder, "include"))
        
        # Copy config files
        copy(self, "*.yaml", os.path.join(self.source_folder, "config"), os.path.join(self.package_folder, "res", "config"))
    
    def package_info(self):
        self.cpp_info.libs = ["thales", "core", "data", "models", "strategies", "utils"]
        
        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.system_libs.extend(["pthread", "m"])
