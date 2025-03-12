#include <thales/utils/config.h>
#include <thales/utils/logger.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>

// Include a JSON library for parsing
// For this example, we'll use a simple placeholder implementation
// In a real project, you would use a proper JSON library like nlohmann/json

namespace thales {
namespace utils {

Config::Config(const std::string& filePath) {
    loadFromFile(filePath);
}

bool Config::loadFromFile(const std::string& filePath) {
    auto& logger = Logger::getInstance();
    logger.info("Loading configuration from: " + filePath);
    
    try {
        // Check if the file exists
        if (!std::filesystem::exists(filePath)) {
            logger.error("Configuration file does not exist: " + filePath);
            return false;
        }
        
        // Open the file
        std::ifstream file(filePath);
        if (!file.is_open()) {
            logger.error("Failed to open configuration file: " + filePath);
            return false;
        }
        
        // Read the file content
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // Parse the JSON content
        // This is a placeholder for actual JSON parsing
        // In a real implementation, you would use a proper JSON library
        
        // For now, we'll just add some dummy values for testing
        setValue("engine.loopIntervalMs", 1000);
        setValue("risk.maxPositionSize", 100000.0);
        setValue("risk.maxDrawdown", 0.05);
        setValue("risk.maxLeverage", 2.0);
        setValue("risk.maxRiskPerTrade", 0.01);
        setValue("risk.maxDailyLoss", 0.02);
        setValue("data.ibHost", std::string("127.0.0.1"));
        setValue("data.ibPort", 7496);
        setValue("data.ibClientId", 1);
        setValue("data.ibAccount", std::string("DU12345"));
        
        logger.info("Configuration loaded successfully");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception while loading configuration: " + std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception while loading configuration");
        return false;
    }
}

bool Config::saveToFile(const std::string& filePath) const {
    auto& logger = Logger::getInstance();
    logger.info("Saving configuration to: " + filePath);
    
    try {
        // Create the directory if it doesn't exist
        std::filesystem::path path(filePath);
        std::filesystem::create_directories(path.parent_path());
        
        // Open the file
        std::ofstream file(filePath);
        if (!file.is_open()) {
            logger.error("Failed to open configuration file for writing: " + filePath);
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
            std::visit([&file](const auto& value) {
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
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << value[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << value[i] << "\"";
                    }
                    file << "]";
                }
            }, pair.second);
        }
        
        file << "\n}\n";
        file.close();
        
        logger.info("Configuration saved successfully");
        return true;
    } catch (const std::exception& e) {
        logger.error("Exception while saving configuration: " + std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception while saving configuration");
        return false;
    }
}

bool Config::hasKey(const std::string& key) const {
    return data_.find(key) != data_.end();
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
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

int Config::getInt(const std::string& key, int defaultValue) const {
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

double Config::getDouble(const std::string& key, double defaultValue) const {
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

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
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

void Config::setValue(const std::string& key, const ConfigValue& value) {
    data_[key] = value;
}

} // namespace utils
} // namespace thales
