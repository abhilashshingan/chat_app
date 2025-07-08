#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <deque>
#include <string>
#include <functional>
#include <mutex>
#include "common/protocol.h"

namespace chat_app {

/**
 * Class representing a TCP connection using Boost.Asio
 * This is the base class for both client and server connections
 */
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    using MessageCallback = std::function<void(const std::vector<char>&, uint16_t, uint16_t)>;
    using ErrorCallback = std::function<void(const boost::system::error_code&)>;
    
    TcpConnection(boost::asio::io_context& io_context);
    virtual ~TcpConnection();
    
    // Socket access
    boost::asio::ip::tcp::socket& socket();
    
    // Start connection
    void start();
    
    // Close connection
    void stop();
    
    // Send a message
    bool send(const std::vector<char>& data, uint16_t type, uint16_t flags = 0);
    
    // Set callbacks
    void setMessageCallback(MessageCallback callback);
    void setErrorCallback(ErrorCallback callback);
    
    // Connection status
    bool isConnected() const;
    std::string getRemoteAddress() const;
    uint16_t getRemotePort() const;

protected:
    // Asynchronous operations
    void asyncReadHeader();
    void asyncReadBody(uint32_t body_size);
    void asyncWrite();
    
    // Callback handlers
    void handleReadHeader(const boost::system::error_code& error);
    void handleReadBody(const boost::system::error_code& error);
    void handleWrite(const boost::system::error_code& error);
    
    // Error handling
    void handleError(const boost::system::error_code& error);
    
    // Member variables
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::socket socket_;
    
    // Read buffer
    std::array<char, HEADER_SIZE> read_header_buffer_;
    std::vector<char> read_body_buffer_;
    MessageHeader current_header_;
    
    // Write queue
    struct OutgoingMessage {
        std::vector<char> header_buffer;
        std::vector<char> body;
    };
    
    std::deque<OutgoingMessage> write_queue_;
    bool write_in_progress_;
    std::mutex write_mutex_;
    
    // Callbacks
    MessageCallback message_callback_;
    ErrorCallback error_callback_;
    bool is_connected_;
};

} // namespace chat_app