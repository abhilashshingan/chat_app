#include "common/chat_message.h"
#include <ctime>
#include <random>

namespace chat_app {

nlohmann::json ChatMessage::toJson() const {
    nlohmann::json json;
    json["id"] = message_id;
    json["sender"] = sender_id;
    json["content"] = content;
    json["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()).count();
    json["type"] = message_type;
    
    if (room_id.has_value()) {
        json["room_id"] = room_id.value();
    }
    
    if (recipient_id.has_value()) {
        json["recipient"] = recipient_id.value();
    }
    
    return json;
}

ChatMessage ChatMessage::fromJson(const nlohmann::json& json) {
    ChatMessage msg;
    
    msg.message_id = json.at("id").get<std::string>();
    msg.sender_id = json.at("sender").get<std::string>();
    msg.content = json.at("content").get<std::string>();
    msg.message_type = json.at("type").get<uint8_t>();
    
    // Convert timestamp from milliseconds to time_point
    int64_t ts = json.at("timestamp").get<int64_t>();
    msg.timestamp = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(ts));
    
    if (json.contains("room_id")) {
        msg.room_id = json["room_id"].get<std::string>();
    }
    
    if (json.contains("recipient")) {
        msg.recipient_id = json["recipient"].get<std::string>();
    }
    
    return msg;
}

std::string ChatMessage::generateUUID() {
    // In real implementation, use a proper UUID library
    // This is just a placeholder implementation
    static uint64_t counter = 0;
    
    // Add some randomness
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    
    // Combine timestamp, counter and random number
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    return "msg_" + std::to_string(timestamp) + 
           "_" + std::to_string(++counter) +
           "_" + std::to_string(dis(gen) % 1000000);
}

void to_json(nlohmann::json& j, const ChatMessage& msg) {
    j = msg.toJson();
}

void from_json(const nlohmann::json& j, ChatMessage& msg) {
    msg = ChatMessage::fromJson(j);
}

} // namespace chat_app