// SPDX-License-Identifier: MIT

#include "thales/utils/config.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "thales/utils/logger.hpp"

namespace thales {
namespace utils {

Config::Config(const std::string &file_path) { load_from_file(file_path); }

namespace {
// Trim whitespace from beginning and end of string
std::string trim(const std::string &str) {
    const auto begin = str.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return "";
    const auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(begin, end - begin + 1);
}

// Remove quotes from a string
std::string remove_quotes(const std::string &str) {
    std::string result = trim(str);
    if (result.size() >= 2 && result.front() == '"' && result.back() == '"') {
        return result.substr(1, result.size() - 2);
    }
    return result;
}

// Parse string to boolean
bool parse_bool(const std::string &str) {
    std::string value = trim(str);
    if (value == "true") return true;
    if (value == "false") return false;
    throw std::runtime_error("Invalid boolean value: " + str);
}

// Parse string to int
int parse_int(const std::string &str) {
    try {
        return std::stoi(trim(str));
    } catch (const std::exception &e) {
        throw std::runtime_error("Invalid integer value: " + str);
    }
}

// Parse string to double
double parse_double(const std::string &str) {
    try {
        return std::stod(trim(str));
    } catch (const std::exception &e) {
        throw std::runtime_error("Invalid double value: " + str);
    }
}

// Parse a YAML array of strings
std::vector<std::string> parse_string_array(const std::string &input) {
    std::vector<std::string> result;
    std::string str = trim(input);

    // YAML format can be:
    // - item1
    // - item2
    // or [item1, item2] (JSON style)

    if (str.size() >= 2 && str.front() == '[' && str.back() == ']') {
        // JSON-style array
        str = str.substr(1, str.size() - 2);

        std::string current;
        bool in_quotes = false;

        for (char c : str) {
            if (c == '"') {
                in_quotes = !in_quotes;
                current += c;
            } else if (c == ',' && !in_quotes) {
                // End of element
                result.push_back(remove_quotes(current));
                current.clear();
            } else {
                current += c;
            }
        }

        // Add the last element if there is one
        if (!current.empty()) {
            result.push_back(remove_quotes(current));
        }
    } else {
        // YAML-style array with dash prefixes
        std::istringstream iss(str);
        std::string line;

        while (std::getline(iss, line)) {
            line = trim(line);
            if (line.empty()) continue;

            // Check if line starts with dash and space
            if (line.size() >= 2 && line.front() == '-' &&
                (line[1] == ' ' || line[1] == '\t')) {
                std::string value = trim(line.substr(2));
                result.push_back(remove_quotes(value));
            }
        }
    }

    return result;
}
}  // namespace

bool Config::load_from_file(const std::string &file_path) {
    auto &logger = Logger::get_instance();
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
        std::string line;
        std::string current_path;
        bool in_array = false;
        std::string array_key;
        std::string array_content;

        while (std::getline(file, line)) {
            // Remove leading/trailing whitespace
            std::string trimmed_line = trim(line);

            // Skip empty lines and comments
            if (trimmed_line.empty() || trimmed_line.front() == '#') {
                continue;
            }

            if (in_array) {
                array_content += line + "\n";
                // Check if this is the end of the array section
                if (line.find('-') == std::string::npos &&
                    (line.find(':') != std::string::npos ||
                     trimmed_line.empty())) {
                    // Process the array
                    std::string full_key = current_path.empty()
                                               ? array_key
                                               : current_path + "." + array_key;
                    set_value(full_key, parse_string_array(array_content));

                    // Reset array state
                    in_array = false;
                    array_key.clear();
                    array_content.clear();

                    // Don't skip this line - it might contain a new key
                }
            }

            // Skip if we're still collecting array lines
            if (in_array) continue;

            // Process normal yaml lines (key: value)
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                // Extract key and value
                std::string key = trim(line.substr(0, colon_pos));
                std::string value = colon_pos + 1 < line.size()
                                        ? trim(line.substr(colon_pos + 1))
                                        : "";

                // Handle nested objects (indentation in YAML)
                unsigned int indent = line.find_first_not_of(" \t");

                // Detect if this is the start of a section
                if (value.empty() && line.back() != ':') {
                    // This is a section header
                    current_path = key;
                    continue;
                }

                // Check if this is an array start
                if ((value.empty() || value.size() >= 2 &&
                                          value.front() == '[' &&
                                          value.back() == ']') &&
                    std::getline(file, line) &&
                    line.find('-') != std::string::npos) {
                    in_array = true;
                    array_key = key;
                    array_content = value.empty() ? line + "\n" : value;
                    continue;
                }

                // Create the full key path
                std::string full_key =
                    current_path.empty() ? key : current_path + "." + key;

                // Parse the value based on its type
                if (value == "true" || value == "false") {
                    set_value(full_key, parse_bool(value));
                } else if (!value.empty() && value.front() == '"' &&
                           value.back() == '"') {
                    set_value(full_key, remove_quotes(value));
                } else if (!value.empty() && value.front() == '[' &&
                           value.back() == ']') {
                    // Handle arrays
                    set_value(full_key, parse_string_array(value));
                } else if (!value.empty() &&
                           value.find('.') != std::string::npos) {
                    // Assume it's a double
                    set_value(full_key, parse_double(value));
                } else if (!value.empty() && std::isdigit(value.front())) {
                    // Assume it's an integer
                    set_value(full_key, parse_int(value));
                } else if (!value.empty()) {
                    // Treat as string
                    set_value(full_key, value);
                }
            }
        }

        file.close();
        logger.info("Configuration loaded successfully");
        return true;
    } catch (const std::exception &e) {
        logger.error("Exception while loading configuration: " +
                     std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception while loading configuration");
        return false;
    }
}

bool Config::save_to_file(const std::string &file_path) const {
    auto &logger = Logger::get_instance();
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

        // Group by top-level sections
        std::unordered_map<std::string,
                           std::vector<std::pair<std::string, ConfigValue>>>
            sections;

        // Find all top-level keys
        for (const auto &[key, value] : data_) {
            size_t dot_pos = key.find('.');

            if (dot_pos != std::string::npos) {
                std::string section = key.substr(0, dot_pos);
                std::string subkey = key.substr(dot_pos + 1);
                sections[section].emplace_back(subkey, value);
            } else {
                // Top-level values go in an empty section
                sections[""].emplace_back(key, value);
            }
        }

        // Write top-level values first
        if (sections.find("") != sections.end()) {
            for (const auto &[key, value] : sections[""]) {
                writeYamlValue(file, key, value, 0);
            }
            file << "\n";
        }

        // Write each section
        for (const auto &[section_name, section_entries] : sections) {
            if (section_name.empty()) continue;  // Already handled

            file << section_name << ":\n";

            for (const auto &[key, value] : section_entries) {
                writeYamlValue(file, key, value, 2);
            }

            file << "\n";
        }

        file.close();
        logger.info("Configuration saved successfully");
        return true;
    } catch (const std::exception &e) {
        logger.error("Exception while saving configuration: " +
                     std::string(e.what()));
        return false;
    } catch (...) {
        logger.error("Unknown exception while saving configuration");
        return false;
    }
}

// Helper to write YAML values with proper formatting
void Config::writeYamlValue(std::ofstream &file, const std::string &key,
                            const ConfigValue &value, int indent) const {
    std::string indent_str(indent, ' ');
    file << indent_str << key << ": ";

    std::visit(
        [&file, &indent_str](const auto &v) {
            using T = std::decay_t<decltype(v)>;

            if constexpr (std::is_same_v<T, bool>) {
                file << (v ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int>) {
                file << v << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << v << "\n";
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Add quotes if needed for strings with special characters
                if (v.find(':') != std::string::npos ||
                    v.find('#') != std::string::npos || v.empty() ||
                    std::isspace(v.front()) || std::isspace(v.back())) {
                    file << "\"" << v << "\"" << "\n";
                } else {
                    file << v << "\n";
                }
            } else if constexpr (std::is_same_v<T, std::vector<bool>> ||
                                 std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                if (v.empty()) {
                    file << "[]\n";
                    return;
                }

                file << "\n";
                for (const auto &item : v) {
                    file << indent_str << "  - ";

                    if constexpr (std::is_same_v<std::decay_t<decltype(item)>,
                                                 bool>) {
                        file << (item ? "true" : "false") << "\n";
                    } else if constexpr (std::is_same_v<
                                             std::decay_t<decltype(item)>,
                                             std::string>) {
                        // Add quotes if needed
                        if (item.find(':') != std::string::npos ||
                            item.find('#') != std::string::npos ||
                            item.empty() || std::isspace(item.front()) ||
                            std::isspace(item.back())) {
                            file << "\"" << item << "\"" << "\n";
                        } else {
                            file << item << "\n";
                        }
                    } else {
                        file << item << "\n";
                    }
                }
            }
        },
        value);
}

bool Config::has_key(const std::string &key) const {
    return data_.find(key) != data_.end();
}

bool Config::get_bool(const std::string &key, bool defaultValue) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return defaultValue;
    }

    try {
        return std::get<bool>(it->second);
    } catch (const std::bad_variant_access &) {
        return defaultValue;
    }
}

int Config::get_int(const std::string &key, int defaultValue) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return defaultValue;
    }

    try {
        return std::get<int>(it->second);
    } catch (const std::bad_variant_access &) {
        return defaultValue;
    }
}

double Config::get_double(const std::string &key, double defaultValue) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return defaultValue;
    }

    try {
        return std::get<double>(it->second);
    } catch (const std::bad_variant_access &) {
        return defaultValue;
    }
}

std::string Config::get_string(const std::string &key,
                               const std::string &defaultValue) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return defaultValue;
    }

    try {
        return std::get<std::string>(it->second);
    } catch (const std::bad_variant_access &) {
        return defaultValue;
    }
}

std::vector<std::string> Config::get_string_vector(
    const std::string &key,
    const std::vector<std::string> &defaultValue) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return defaultValue;
    }

    try {
        return std::get<std::vector<std::string>>(it->second);
    } catch (const std::bad_variant_access &) {
        return defaultValue;
    }
}

void Config::set_value(const std::string &key, const ConfigValue &value) {
    data_[key] = value;
}

}  // namespace utils
}  // namespace thales
