// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace thales {
namespace utils {

/**
 * @class HardwareInfo
 * @brief Provides information about the system hardware
 *
 * This class allows the trading system to query information about the hardware
 * it's running on, which can be useful for performance tuning and logging.
 */
class HardwareInfo {
  public:
    /**
     * @brief Get the singleton instance of HardwareInfo
     * @return Reference to the HardwareInfo instance
     */
    static HardwareInfo &get_instance();

    /**
     * @brief Get the number of CPU cores available
     * @return Number of CPU cores
     */
    int get_cpu_core_count() const;

    /**
     * @brief Get the total system memory in bytes
     * @return Total system memory in bytes
     */
    uint64_t get_total_memory() const;

    /**
     * @brief Get the available system memory in bytes
     * @return Available system memory in bytes
     */
    uint64_t get_available_memory() const;

    /**
     * @brief Get CPU usage as a percentage
     * @return CPU usage percentage (0-100)
     */
    double get_cpu_usage() const;

    /**
     * @brief Get memory usage as a percentage
     * @return Memory usage percentage (0-100)
     */
    double get_memory_usage() const;

    /**
     * @brief Get CPU model name
     * @return CPU model name as a string
     */
    std::string get_cpu_model() const;

  private:
    // Private constructor to enforce singleton pattern
    HardwareInfo();

    // Delete copy constructor and assignment operator
    HardwareInfo(const HardwareInfo &) = delete;
    HardwareInfo &operator=(const HardwareInfo &) = delete;

    // Singleton instance
    static std::unique_ptr<HardwareInfo> instance_;

    // CPU info
    int cpu_core_count_;
    std::string cpu_model_;

    // Memory info
    uint64_t total_memory_;

    // Private helper methods
    void update_cpu_info();
    void update_memory_info();
};

} // namespace utils
} // namespace thales
