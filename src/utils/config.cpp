#include <thales/utils/config.h>
#include <thales/utils/logger.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

// Include a JSON library for parsing
// For this example, we'll use a simple placeholder implementation
// In a real project, you would use a proper JSON library like nlohmann/json

namespace thales {
namespace utils {

Config::Config(const std::string& file_path) { load_from_file(file_path); }

bool Config::load_from_file(const std::string& file_path) {
    auto& logger = Logger::get_instance();
    logger.info("Loading configuration from: " + file_path);

    try {
        // Check if the file exists
        if (!std::filesystem::exists(file_path)) {
            logger.error("Configuration file does not exist: " + file_path);
            return false;
        }

        // Open the file
        std::ifstream file(file_path);
        if (!file.is_open()) {
            logger.error("Failed to open configuration file: " + file_path);
            return false;
        }

        // Read the file content
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        // Parse the JSON content
        // This is a placeholder for actual JSON parsing
        // In a real implementation, you would use a proper JSON library

        // For now, we'll just add some dummy values for testing
        set_value("engine.loopIntervalMs", 1000);
        set_value("risk.maxPositionSize", 100000.0);
        set_value("risk.maxDrawdown", 0.05);
        set_value("risk.maxLeverage", 2.0);
        set_value("risk.maxRiskPerTrade", 0.01);
        set_value("risk.maxDailyLoss", 0.02);
        set_value("data.ibHost", std::string("127.0.0.1"));
        set_value("data.ibPort", 7496);
        set_value("data.ibClientId", 1);
        set_value("data.ibAccount", std::string("DU12345"));

        logger.info("Configuration loaded successfully");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception while loading configuration: " +
                     std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception while loading configuration");
        return false;
    }
}

bool Config::save_to_file(const std::string& file_path) const {
    auto& logger = Logger::get_instance();
    logger.info("Saving configuration to: " + file_path);

    try {
        // Create the directory if it doesn't exist
        std::filesystem::path path(file_path);
        std::filesystem::create_directories(path.parent_path());

        // Open the file
        std::ofstream file(file_path);
        if (!file.is_open()) {
            logger.error("Failed to open configuration file for writing: " +
                         file_path);
            return false;
        }

        // Serialize the configuration to JSON
        // This is a placeholder for actual JSON serialization
        // In a real implementation, you would use a proper JSON library

        file << "{\n";

        // Serialize each key-value pair
        bool first = true;
        for (const auto& pair : data_) {
            if (!first) {
                file << ",\n";
            }
            first = false;

            file << "  \"" << pair.first << "\": ";

            // Serialize the value based on its type
            std::visit(
                [&file](const auto& value) {
                    using T = std::decay_t<decltype(value)>;

                    if constexpr (std::is_same_v<T, bool>) {
                        file << (value ? "true" : "false");
                    } else if constexpr (std::is_same_v<T, int>) {
                        file << value;
                    } else if constexpr (std::is_same_v<T, double>) {
                        file << value;
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        file << "\"" << value << "\"";
                    } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                        file << "[";
                        for (size_t i = 0; i < value.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << (value[i] ? "true" : "false");
                        }
                        file << "]";
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        file << "[";
                        for (size_t i = 0; i < value.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << value[i];
                        }
                        file << "]";
                    } else if constexpr (std::is_same_v<T,
                                                        std::vector<double>>) {
                        file << "[";
                        for (size_t i = 0; i < value.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << value[i];
                        }
                        file << "]";
                    } else if constexpr (std::is_same_v<
                                             T, std::vector<std::string>>) {
                        file << "[";
                        for (size_t i = 0; i < value.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << "\"" << value[i] << "\"";
                        }
                        file << "]";
                    }
                },
                pair.second);
        }

        file << "\n}\n";
        file.close();

        logger.info("Configuration saved successfully");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception while saving configuration: " +
                     std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception while saving configuration");
        return false;
    }
}

bool Config::has_key(const std::string& key) const {
    return data_.find(key) != data_.end();
}

bool Config::get_bool(const std::string& key, bool defaultValue) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return defaultValue;
    }

    try {
        return std::get<bool>(it->second);
    } catch (const std::bad_variant_access&) {
        return defaultValue;
    }
}

int Config::get_int(const std::string& key, int defaultValue) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return defaultValue;
    }

    try {
        return std::get<int>(it->second);
    } catch (const std::bad_variant_access&) {
        return defaultValue;
    }
}

double Config::get_double(const std::string& key, double defaultValue) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return defaultValue;
    }

    try {
        return std::get<double>(it->second);
    } catch (const std::bad_variant_access&) {
        return defaultValue;
    }
}

std::string Config::get_string(const std::string& key,
                              const std::string& defaultValue) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return defaultValue;
    }

    try {
        return std::get<std::string>(it->second);
    } catch (const std::bad_variant_access&) {
        return defaultValue;
    }
}

void Config::set_value(const std::string& key, const ConfigValue& value) {
    data_[key] = value;
}

}  // namespace utils
}  // namespace thales
