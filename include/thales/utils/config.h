#ifndef THALES_UTILS_CONFIG_H
#define THALES_UTILS_CONFIG_H

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace thales {
namespace utils {

/**
 * @class Config
 * @brief Configuration management class for the trading bot.
 * 
 * This class handles loading, parsing, and accessing configuration settings
 * from JSON files or other sources.
 */
class Config {
public:
    // Define the types that can be stored in the configuration
    using ConfigValue = std::variant<
        bool,
        int,
        double,
        std::string,
        std::vector<bool>,
        std::vector<int>,
        std::vector<double>,
        std::vector<std::string>
    >;
    
    /**
     * @brief Default constructor
     */
    Config() = default;
    
    /**
     * @brief Constructor that loads configuration from a file
     * @param filePath Path to the configuration file
     */
    explicit Config(const std::string& filePath);
    
    /**
     * @brief Load configuration from a file
     * @param filePath Path to the configuration file
     * @return true if loading was successful, false otherwise
     */
    bool loadFromFile(const std::string& filePath);
    
    /**
     * @brief Save configuration to a file
     * @param filePath Path to the configuration file
     * @return true if saving was successful, false otherwise
     */
    bool saveToFile(const std::string& filePath) const;
    
    /**
     * @brief Check if a configuration key exists
     * @param key The configuration key to check
     * @return true if the key exists, false otherwise
     */
    bool hasKey(const std::string& key) const;
    
    /**
     * @brief Get a boolean value from the configuration
     * @param key The configuration key
     * @param defaultValue The default value to return if the key doesn't exist
     * @return The boolean value
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    /**
     * @brief Get an integer value from the configuration
     * @param key The configuration key
     * @param defaultValue The default value to return if the key doesn't exist
     * @return The integer value
     */
    int getInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * @brief Get a double value from the configuration
     * @param key The configuration key
     * @param defaultValue The default value to return if the key doesn't exist
     * @return The double value
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    
    /**
     * @brief Get a string value from the configuration
     * @param key The configuration key
     * @param defaultValue The default value to return if the key doesn't exist
     * @return The string value
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * @brief Set a configuration value
     * @param key The configuration key
     * @param value The value to set
     */
    void setValue(const std::string& key, const ConfigValue& value);
    
private:
    std::unordered_map<std::string, ConfigValue> data_;
};

} // namespace utils
} // namespace thales

#endif // THALES_UTILS_CONFIG_H
