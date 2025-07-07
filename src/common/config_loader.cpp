#include "common/config_loader.h"
#include <algorithm>
#include <cctype>

namespace chat {

ConfigLoader::ConfigLoader(const std::string& config_file_path)
    : config_file_path_(config_file_path) {
    reload();
}

bool ConfigLoader::reload() {
    config_values_.clear();
    
    std::ifstream config_file(config_file_path_);
    if (!config_file.is_open()) {
        std::cerr << "Error: Could not open config file: " << config_file_path_ << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(config_file, line)) {
        parseLine(line);
    }
    
    return true;
}

void ConfigLoader::parseLine(const std::string& line) {
    // Skip empty lines and comments
    std::string trimmed_line = trim(line);
    if (trimmed_line.empty() || trimmed_line[0] == '#') {
        return;
    }
    
    // Find the equals sign separating key and value
    size_t equals_pos = trimmed_line.find('=');
    if (equals_pos == std::string::npos) {
        return; // Skip lines without equals sign
    }
    
    // Extract and trim key and value
    std::string key = trim(trimmed_line.substr(0, equals_pos));
    std::string value_with_comment = trimmed_line.substr(equals_pos + 1);
    
    // Remove inline comments
    size_t comment_pos = value_with_comment.find('#');
    std::string value;
    if (comment_pos != std::string::npos) {
        value = trim(value_with_comment.substr(0, comment_pos));
    } else {
        value = trim(value_with_comment);
    }
    
    // Store in the map if key is not empty
    if (!key.empty()) {
        config_values_[key] = value;
    }
}

std::string ConfigLoader::getString(const std::string& key, const std::string& default_value) const {
    auto it = config_values_.find(key);
    if (it != config_values_.end()) {
        return it->second;
    }
    return default_value;
}

int ConfigLoader::getInt(const std::string& key, int default_value) const {
    auto it = config_values_.find(key);
    if (it != config_values_.end()) {
        try {
            return std::stoi(it->second);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to convert '" << it->second << "' to int for key '" << key << "': " << e.what() << std::endl;
        }
    }
    return default_value;
}

bool ConfigLoader::getBool(const std::string& key, bool default_value) const {
    auto it = config_values_.find(key);
    if (it != config_values_.end()) {
        std::string value_lower = it->second;
        std::transform(value_lower.begin(), value_lower.end(), value_lower.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
                      
        if (value_lower == "true" || value_lower == "yes" || value_lower == "1") {
            return true;
        } else if (value_lower == "false" || value_lower == "no" || value_lower == "0") {
            return false;
        }
        std::cerr << "Warning: Failed to convert '" << it->second << "' to bool for key '" << key << "'" << std::endl;
    }
    return default_value;
}

double ConfigLoader::getFloat(const std::string& key, double default_value) const {
    auto it = config_values_.find(key);
    if (it != config_values_.end()) {
        try {
            return std::stod(it->second);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to convert '" << it->second << "' to float for key '" << key << "': " << e.what() << std::endl;
        }
    }
    return default_value;
}

LogLevel ConfigLoader::getLogLevel() const {
    return stringToLogLevel(getString("LOG_LEVEL", "INFO"));
}

bool ConfigLoader::hasKey(const std::string& key) const {
    return config_values_.find(key) != config_values_.end();
}

void ConfigLoader::dumpConfig(std::ostream& out) const {
    out << "Configuration (" << config_file_path_ << "):\n";
    for (const auto& [key, value] : config_values_) {
        out << "  " << key << " = " << value << "\n";
    }
}

LogLevel ConfigLoader::stringToLogLevel(const std::string& level_str) const {
    std::string upper_level = level_str;
    std::transform(upper_level.begin(), upper_level.end(), upper_level.begin(), 
                  [](unsigned char c) { return std::toupper(c); });
                  
    if (upper_level == "TRACE") return LogLevel::TRACE;
    if (upper_level == "DEBUG") return LogLevel::DEBUG;
    if (upper_level == "INFO") return LogLevel::INFO;
    if (upper_level == "WARN" || upper_level == "WARNING") return LogLevel::WARN;
    if (upper_level == "ERROR") return LogLevel::ERROR;
    
    // Default to INFO if the level is not recognized
    std::cerr << "Warning: Unknown log level '" << level_str << "', defaulting to INFO" << std::endl;
    return LogLevel::INFO;
}

std::string ConfigLoader::trim(const std::string& str) const {
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char c) {
        return std::isspace(c);
    });
    
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
        return std::isspace(c);
    }).base();
    
    return (start < end) ? std::string(start, end) : std::string();
}

} // namespace chat