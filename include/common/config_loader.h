#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>
#include <filesystem>

namespace chat {

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class ConfigLoader {
public:
    // Constructor loads config from the specified file
    explicit ConfigLoader(const std::string& config_file_path);
    
    // Reload configuration from file
    bool reload();
    
    // Get config values of different types with default fallbacks
    std::string getString(const std::string& key, const std::string& default_value = "") const;
    int getInt(const std::string& key, int default_value = 0) const;
    bool getBool(const std::string& key, bool default_value = false) const;
    double getFloat(const std::string& key, double default_value = 0.0) const;
    
    // Specific getters for common configuration values
    int getServerPort() const { return getInt("SERVER_PORT", 8080); }
    int getAutosaveInterval() const { return getInt("AUTOSAVE_INTERVAL", 300); }
    LogLevel getLogLevel() const;
    
    // Check if a key exists
    bool hasKey(const std::string& key) const;
    
    // Dump all configuration values (useful for debugging)
    void dumpConfig(std::ostream& out = std::cout) const;

private:
    std::string config_file_path_;
    std::unordered_map<std::string, std::string> config_values_;
    
    // Parse a single line from the config file
    void parseLine(const std::string& line);
    
    // Convert string to LogLevel
    LogLevel stringToLogLevel(const std::string& level_str) const;
    
    // Trim whitespace from a string
    std::string trim(const std::string& str) const;
};

} // namespace chat