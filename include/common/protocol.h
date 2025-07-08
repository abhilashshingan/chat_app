pragma once
#include <cstdint>
#include <boost/asio.hpp>
#include <array>
#include <string>

namespace chat_app {

// Protocol constants
constexpr std::size_t HEADER_SIZE = 12;  // Fixed header size in bytes
constexpr std::size_t MAX_BODY_SIZE = 1024 * 1024;  // 1MB maximum message size

/**
 * Structure representing a message header in the chat protocol.
 * Header format:
 * - 4 bytes: Magic number (helps detect corrupt/invalid messages)
 * - 2 bytes: Message type
 * - 2 bytes: Flags
 * - 4 bytes: Payload size (uint32_t)
 */
class MessageHeader {
public:
    MessageHeader();
    
    // Set message size
    void setBodySize(uint32_t size);
    
    // Get message size
    uint32_t getBodySize() const;
    
    // Set message type 
    void setMessageType(uint16_t type);
    
    // Get message type
    uint16_t getMessageType() const;
    
    // Set flags
    void setFlags(uint16_t flags);
    
    // Get flags
    uint16_t getFlags() const;
    
    // Validate header
    bool isValid() const;
    
    // Encode/decode with network byte order
    void encodeToBuffer(std::array<char, HEADER_SIZE>& buffer);
    void decodeFromBuffer(const std::array<char, HEADER_SIZE>& buffer);

private:
    static constexpr uint32_t MAGIC_NUMBER = 0x43484154; // "CHAT" in ASCII
    uint32_t magic_number_ = MAGIC_NUMBER;
    uint16_t message_type_ = 0;
    uint16_t flags_ = 0;
    uint32_t body_size_ = 0;
};

/**
 * Common flag definitions for protocol
 */
namespace MessageFlags {
    constexpr uint16_t COMPRESSED = 0x0001;   // Message body is compressed
    constexpr uint16_t ENCRYPTED  = 0x0002;   // Message body is encrypted
    constexpr uint16_t URGENT     = 0x0004;   // High priority message
    constexpr uint16_t ACK_REQ    = 0x0008;   // Acknowledgment requested
    constexpr uint16_t JSON       = 0x0010;   // Body is JSON formatted
    constexpr uint16_t BINARY     = 0x0020;   // Body is binary data
    constexpr uint16_t FRAGMENT   = 0x0040;   // Message is a fragment of a larger message
    constexpr uint16_t LAST_FRAG  = 0x0080;   // Last fragment in a message
}

} // namespace chat_app