#pragma once
#include <string>
#include <optional>
#include <chrono>
#include <nlohmann/json.hpp>
#include <vector>
#include <algorithm>

namespace chat_app {

/**
 * Enumeration for user status in the chat application
 */
enum class UserStatus {
    OFFLINE = 0,
    ONLINE = 1, 
    AWAY = 2,
    DO_NOT_DISTURB = 3
};

/**
 * Lightweight struct for user representation
 * Optimized for JSON serialization/deserialization and memory efficiency
 */
struct User {
    // Required fields
    std::string user_id;                  // Unique user identifier
    std::string username;                 // Login username
    UserStatus status = UserStatus::OFFLINE; // Current session status
    
    // Optional fields
    std::optional<std::string> display_name;    // Display name (optional)
    std::optional<std::string> email;           // User email (optional)
    std::optional<std::string> avatar_url;      // User avatar (optional)
    std::optional<std::chrono::system_clock::time_point> last_seen; // Last activity timestamp
    
    // Vector of room IDs this user belongs to
    std::vector<std::string> room_ids;
    
    // Constructors
    User() = default;
    
    // Constructor with required fields
    User(
        std::string id,
        std::string name,
        UserStatus user_status = UserStatus::OFFLINE
    ) : user_id(std::move(id)),
        username(std::move(name)),
        status(user_status) {
    }
    
    // JSON serialization/deserialization
    nlohmann::json toJson() const;
    static User fromJson(const nlohmann::json& json);
    
    // Helper methods
    bool isOnline() const { return status == UserStatus::ONLINE; }
    std::string getDisplayName() const { 
        return display_name.value_or(username); 
    }
    
    // Add user to a room
    void addToRoom(const std::string& room_id) {
        // Check if room_id already exists in the vector
        if (std::find(room_ids.begin(), room_ids.end(), room_id) == room_ids.end()) {
            room_ids.push_back(room_id);
        }
    }
    
    // Remove user from a room
    void removeFromRoom(const std::string& room_id) {
        auto it = std::find(room_ids.begin(), room_ids.end(), room_id);
        if (it != room_ids.end()) {
            room_ids.erase(it);
        }
    }
    
    // Check if user is in a room
    bool isInRoom(const std::string& room_id) const {
        return std::find(room_ids.begin(), room_ids.end(), room_id) != room_ids.end();
    }
};

// JSON serialization support for nlohmann/json
void to_json(nlohmann::json& j, const User& user);
void from_json(const nlohmann::json& j, User& user);

// JSON serialization for UserStatus enum
NLOHMANN_JSON_SERIALIZE_ENUM(UserStatus, {
    {UserStatus::OFFLINE, "offline"},
    {UserStatus::ONLINE, "online"},
    {UserStatus::AWAY, "away"},
    {UserStatus::DO_NOT_DISTURB, "dnd"}
});

} // namespace chat_app