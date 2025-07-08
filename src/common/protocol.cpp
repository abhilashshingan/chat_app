#include "common/protocol.h"
#include <cstring>
#include <arpa/inet.h>  // For network byte order conversions
#include <stdexcept>

namespace chat_app {

MessageHeader::MessageHeader() : 
    magic_number_(MAGIC_NUMBER), 
    message_type_(0), 
    flags_(0), 
    body_size_(0) {
}

void MessageHeader::setBodySize(uint32_t size) {
    if (size > MAX_BODY_SIZE) {
        throw std::runtime_error("Message body size exceeds maximum allowed size");
    }
    body_size_ = size;
}

uint32_t MessageHeader::getBodySize() const {
    return body_size_;
}

void MessageHeader::setMessageType(uint16_t type) {
    message_type_ = type;
}

uint16_t MessageHeader::getMessageType() const {
    return message_type_;
}

void MessageHeader::setFlags(uint16_t flags) {
    flags_ = flags;
}

uint16_t MessageHeader::getFlags() const {
    return flags_;
}

bool MessageHeader::isValid() const {
    // Check magic number and size constraints
    return (magic_number_ == MAGIC_NUMBER && body_size_ <= MAX_BODY_SIZE);
}

void MessageHeader::encodeToBuffer(std::array<char, HEADER_SIZE>& buffer) {
    // Convert to network byte order for transmission
    uint32_t net_magic = htonl(magic_number_);
    uint16_t net_type = htons(message_type_);
    uint16_t net_flags = htons(flags_);
    uint32_t net_size = htonl(body_size_);
    
    // Copy to buffer
    std::memcpy(buffer.data(), &net_magic, sizeof(net_magic));
    std::memcpy(buffer.data() + 4, &net_type, sizeof(net_type));
    std::memcpy(buffer.data() + 6, &net_flags, sizeof(net_flags));
    std::memcpy(buffer.data() + 8, &net_size, sizeof(net_size));
}

void MessageHeader::decodeFromBuffer(const std::array<char, HEADER_SIZE>& buffer) {
    // Read from buffer
    uint32_t net_magic;
    uint16_t net_type;
    uint16_t net_flags;
    uint32_t net_size;
    
    std::memcpy(&net_magic, buffer.data(), sizeof(net_magic));
    std::memcpy(&net_type, buffer.data() + 4, sizeof(net_type));
    std::memcpy(&net_flags, buffer.data() + 6, sizeof(net_flags));
    std::memcpy(&net_size, buffer.data() + 8, sizeof(net_size));
    
    // Convert from network byte order
    magic_number_ = ntohl(net_magic);
    message_type_ = ntohs(net_type);
    flags_ = ntohs(net_flags);
    body_size_ = ntohl(net_size);
}

} // namespace chat_app