#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>

namespace chat_app {

// Forward declarations
class User;
class ChatRoom;

/**
 * Enumeration for message types in the chat application
 */
enum class MessageType {
    AUTH_REQUEST,       // Authentication request
    AUTH_RESPONSE,      // Authentication response
    USER_STATUS,        // User online status update
    TEXT_MESSAGE,       // Regular text message
    GROUP_MESSAGE,      // Message to a chat room
    FILE_TRANSFER,      // File transfer
    TYPING_INDICATOR,   // Typing indicator
    READ_RECEIPT,       // Message read receipt
    JOIN_ROOM,          // Request to join a chat room
    LEAVE_ROOM,         // Request to leave a chat room
    CREATE_ROOM,        // Request to create a chat room
    ERROR              // Error message
};

/**
 * Enumeration for user status in the chat application
 */
enum class UserStatus {
    OFFLINE,
    ONLINE,
    AWAY,
    DO_NOT_DISTURB    
};

/**
 * Class representing a user in the chat application
 */
class User {
public:
    User(const std::string& user_id, const std::string& username);
    
    // Getters
    const std::string& getUserId() const { return user_id_; }
    const std::string& getUsername() const { return username_; }
    UserStatus getStatus() const { return status_; }
    
    // Setters
    void setStatus(UserStatus status) { status_ = status; }
    void setDisplayName(const std::string& display_name) { display_name_ = display_name; }
    
    // User profile management
    void setProfileData(const std::string& key, const nlohmann::json& value);
    nlohmann::json getProfileData(const std::string& key) const;
    
    // Room management
    void joinRoom(std::shared_ptr<ChatRoom> room);
    void leaveRoom(const std::string& room_id);
    std::vector<std::string> getRoomIds() const;
    
    // Serialize to JSON
    nlohmann::json toJson() const;
    static User fromJson(const nlohmann::json& json);

private:
    std::string user_id_;                          // Unique identifier
    std::string username_;                         // Username for login
    std::string display_name_;                     // Display name
    UserStatus status_ = UserStatus::OFFLINE;      // Current user status
    nlohmann::json profile_data_;                  // User profile data
    std::unordered_map<std::string, std::weak_ptr<ChatRoom>> rooms_; // Rooms the user is in
};

/**
 * Class representing a chat room in the application
 */
class ChatRoom {
public:
    ChatRoom(const std::string& room_id, const std::string& name, const std::string& creator_id);
    
    // Room information
    const std::string& getRoomId() const { return room_id_; }
    const std::string& getName() const { return name_; }
    const std::string& getCreatorId() const { return creator_id_; }
    
    // Room management
    bool addMember(std::shared_ptr<User> user);
    bool removeMember(const std::string& user_id);
    bool hasMember(const std::string& user_id) const;
    std::vector<std::string> getMemberIds() const;
    
    // Room settings
    void setDescription(const std::string& description) { description_ = description; }
    const std::string& getDescription() const { return description_; }
    void setIsPrivate(bool is_private) { is_private_ = is_private; }
    bool isPrivate() const { return is_private_; }
    
    // Serialize to JSON
    nlohmann::json toJson() const;
    static ChatRoom fromJson(const nlohmann::json& json);

private:
    std::string room_id_;                            // Unique room identifier
    std::string name_;                               // Room name
    std::string description_;                        // Room description
    std::string creator_id_;                         // User ID of creator
    bool is_private_ = false;                        // Is this room private
    std::chrono::system_clock::time_point created_at_; // Creation timestamp
    std::unordered_map<std::string, std::weak_ptr<User>> members_; // Room members
};

/**
 * Class representing a message in the chat application
 */
class Message {
public:
    Message(MessageType type, 
            const std::string& sender_id, 
            const std::string& recipient_id,
            const std::string& content);
    
    // Create message from JSON
    static Message fromJson(const std::string& json_str);
    
    // Serialize to JSON
    std::string toJson() const;
    
    // Getters
    MessageType getType() const { return type_; }
    std::string getSenderId() const { return sender_id_; }
    std::string getRecipientId() const { return recipient_id_; }
    std::string getContent() const { return content_; }
    std::string getMessageId() const { return message_id_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    bool isRoomMessage() const { return is_room_message_; }
    
    // Setters
    void setRoomMessage(bool is_room_message) { is_room_message_ = is_room_message; }
    
    // Metadata handling
    void setMetadata(const std::string& key, const nlohmann::json& value);
    nlohmann::json getMetadata(const std::string& key) const;

private:
    MessageType type_;
    std::string sender_id_;
    std::string recipient_id_;
    std::string content_;
    std::string message_id_;  // Unique message identifier
    std::chrono::system_clock::time_point timestamp_;
    bool is_room_message_ = false; // Whether this is a message to a room
    nlohmann::json metadata_;  // Additional message metadata
};

} // namespace chat_app