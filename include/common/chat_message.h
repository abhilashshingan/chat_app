#pragma once
#include <string>
#include <chrono>
#include <optional>
#include <nlohmann/json.hpp>

namespace chat_app {

/**
 * Lightweight struct for message representation
 * Optimized for JSON serialization/deserialization and memory efficiency
 */
struct ChatMessage {
    // Required fields
    std::string message_id;                        // Unique message identifier
    std::string sender_id;                         // User ID of sender
    std::string content;                           // Message content
    std::chrono::system_clock::time_point timestamp; // Timestamp when message was created
    
    // Optional fields
    std::optional<std::string> room_id;            // Room ID if it's a room message (nullptr for direct messages)
    std::optional<std::string> recipient_id;       // Recipient user ID for direct messages
    uint8_t message_type = 0;                      // Type of message (using compact uint8_t)
    
    // Constructors
    ChatMessage() = default;
    
    // Constructor for direct messages
    ChatMessage(
        std::string sender,
        std::string recipient,
        std::string content_text,
        uint8_t type = 0
    ) : sender_id(std::move(sender)),
        recipient_id(std::move(recipient)),
        content(std::move(content_text)),
        message_type(type),
        timestamp(std::chrono::system_clock::now()) {
        // Generate random UUID or use a library like uuid-dev
        message_id = generateUUID();
    }
    
    // Constructor for room messages
    ChatMessage(
        std::string sender,
        std::string room,
        std::string content_text,
        uint8_t type = 0
    ) : sender_id(std::move(sender)), 
        room_id(std::move(room)),
        content(std::move(content_text)),
        message_type(type),
        timestamp(std::chrono::system_clock::now()) {
        message_id = generateUUID();
    }
    
    // JSON serialization/deserialization
    nlohmann::json toJson() const;
    static ChatMessage fromJson(const nlohmann::json& json);
    
    // Generate a random UUID (simplified implementation)
    static std::string generateUUID();
    
    // Helper methods
    bool isRoomMessage() const { return room_id.has_value(); }
    bool isDirectMessage() const { return recipient_id.has_value(); }
};

// JSON serialization support for nlohmann::json
void to_json(nlohmann::json& j, const ChatMessage& msg);
void from_json(const nlohmann::json& j, ChatMessage& msg);

} // namespace chat_app