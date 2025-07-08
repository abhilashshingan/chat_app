#include "common/user.h"

namespace chat_app {

nlohmann::json User::toJson() const {
    nlohmann::json json;
    
    // Required fields
    json["user_id"] = user_id;
    json["username"] = username;
    json["status"] = status;
    
    // Optional fields
    if (display_name.has_value()) {
        json["display_name"] = display_name.value();
    }
    
    if (email.has_value()) {
        json["email"] = email.value();
    }
    
    if (avatar_url.has_value()) {
        json["avatar_url"] = avatar_url.value();
    }
    
    if (last_seen.has_value()) {
        json["last_seen"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            last_seen.value().time_since_epoch()).count();
    }
    
    // Room IDs
    json["room_ids"] = room_ids;
    
    return json;
}

User User::fromJson(const nlohmann::json& json) {
    User user;
    
    // Required fields
    user.user_id = json.at("user_id").get<std::string>();
    user.username = json.at("username").get<std::string>();
    user.status = json.at("status").get<UserStatus>();
    
    // Optional fields
    if (json.contains("display_name")) {
        user.display_name = json["display_name"].get<std::string>();
    }
    
    if (json.contains("email")) {
        user.email = json["email"].get<std::string>();
    }
    
    if (json.contains("avatar_url")) {
        user.avatar_url = json["avatar_url"].get<std::string>();
    }
    
    if (json.contains("last_seen")) {
        int64_t ts = json["last_seen"].get<int64_t>();
        user.last_seen = std::chrono::system_clock::time_point(
            std::chrono::milliseconds(ts));
    }
    
    // Room IDs
    if (json.contains("room_ids")) {
        user.room_ids = json["room_ids"].get<std::vector<std::string>>();
    }
    
    return user;
}

void to_json(nlohmann::json& j, const User& user) {
    j = user.toJson();
}

void from_json(const nlohmann::json& j, User& user) {
    user = User::fromJson(j);
}

} // namespace chat_app