// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace YAML {
class Node;
}

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
    using ConfigValue =
        std::variant<bool, int, double, std::string, std::vector<bool>,
                     std::vector<int>, std::vector<double>,
                     std::vector<std::string>>;

    /**
     * @brief Default constructor
     */
    Config() = default;

    /**
     * @brief Constructor that loads configuration from a file
     * @param file_path Path to the configuration file
     */
    explicit Config(const std::string &file_path);

    /**
     * @brief Load configuration from a file
     * @param file_path Path to the configuration file
     * @return true if loading was successful, false otherwise
     */
    bool load_from_file(const std::string &file_path);

    /**
     * @brief Save configuration to a file
     * @param file_path Path to the configuration file
     * @return true if saving was successful, false otherwise
     */
    bool save_to_file(const std::string &file_path) const;

    /**
     * @brief Check if a configuration key exists
     * @param key The configuration key to check
     * @return true if the key exists, false otherwise
     */
    bool has_key(const std::string &key) const;

    /**
     * @brief Get a boolean value from the configuration
     * @param key The configuration key
     * @param defaultValue The default value to return if the key doesn't
     * exist
     * @return The boolean value
     */
    bool get_bool(const std::string &key, bool defaultValue = false) const;

    /**
     * @brief Get an integer value from the configuration
     * @param key The configuration key
     * @param defaultValue The default value to return if the key doesn't
     * exist
     * @return The integer value
     */
    int get_int(const std::string &key, int defaultValue = 0) const;

    /**
     * @brief Get a double value from the configuration
     * @param key The configuration key
     * @param defaultValue The default value to return if the key doesn't
     * exist
     * @return The double value
     */
    double get_double(const std::string &key, double defaultValue = 0.0) const;

    /**
     * @brief Get a string value from the configuration
     * @param key The configuration key
     * @param defaultValue The default value to return if the key doesn't
     * exist
     * @return The string value
     */
    std::string get_string(const std::string &key,
                           const std::string &defaultValue = "") const;

    /**
     * @brief Get a vector of strings from the configuration
     * @param key The configuration key
     * @param defaultValue The default value to return if the key doesn't
     * exist
     * @return The vector of strings
     */
    std::vector<std::string>
    get_string_vector(const std::string &key,
                      const std::vector<std::string> &defaultValue = {}) const;

    /**
     * @brief Set a configuration value
     * @param key The configuration key
     * @param value The value to set
     */
    void set_value(const std::string &key, const ConfigValue &value);

  private:
    // Helper to write YAML values with proper formatting
    void writeYamlValue(std::ofstream &file, const std::string &key,
                        const ConfigValue &value, int indent) const;

    std::unordered_map<std::string, ConfigValue> data_;
};

} // namespace utils
} // namespace thales
