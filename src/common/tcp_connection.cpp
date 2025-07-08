#include "common/tcp_connection.h"
#include <iostream>

namespace chat_app {

TcpConnection::TcpConnection(boost::asio::io_context& io_context)
    : io_context_(io_context),
      socket_(io_context),
      write_in_progress_(false),
      is_connected_(false) {
}

TcpConnection::~TcpConnection() {
    stop();
}

boost::asio::ip::tcp::socket& TcpConnection::socket() {
    return socket_;
}

void TcpConnection::start() {
    is_connected_ = true;
    asyncReadHeader();
}

void TcpConnection::stop() {
    boost::system::error_code ignored_error;
    
    if (socket_.is_open()) {
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_error);
        socket_.close(ignored_error);
    }
    
    is_connected_ = false;
}

bool TcpConnection::send(const std::vector<char>& data, uint16_t type, uint16_t flags) {
    if (!is_connected_) {
        return false;
    }
    
    // Create header and encode it
    MessageHeader header;
    header.setMessageType(type);
    header.setFlags(flags);
    header.setBodySize(data.size());
    
    // Prepare message for queuing
    OutgoingMessage message;
    message.header_buffer.resize(HEADER_SIZE);
    std::array<char, HEADER_SIZE> header_buffer;
    header.encodeToBuffer(header_buffer);
    std::copy(header_buffer.begin(), header_buffer.end(), message.header_buffer.begin());
    message.body = data;
    
    // Queue the message
    {
        std::lock_guard<std::mutex> lock(write_mutex_);
        bool write_in_progress = !write_queue_.empty();
        write_queue_.push_back(std::move(message));
        
        if (!write_in_progress) {
            asyncWrite();
        }
    }
    
    return true;
}

void TcpConnection::setMessageCallback(MessageCallback callback) {
    message_callback_ = callback;
}

void TcpConnection::setErrorCallback(ErrorCallback callback) {
    error_callback_ = callback;
}

bool TcpConnection::isConnected() const {
    return is_connected_ && socket_.is_open();
}

std::string TcpConnection::getRemoteAddress() const {
    if (!is_connected_) {
        return "";
    }
    
    boost::system::error_code error;
    auto endpoint = socket_.remote_endpoint(error);
    if (error) {
        return "";
    }
    
    return endpoint.address().to_string();
}

uint16_t TcpConnection::getRemotePort() const {
    if (!is_connected_) {
        return 0;
    }
    
    boost::system::error_code error;
    auto endpoint = socket_.remote_endpoint(error);
    if (error) {
        return 0;
    }
    
    return endpoint.port();
}

void TcpConnection::asyncReadHeader() {
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(read_header_buffer_, HEADER_SIZE),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            handleReadHeader(ec);
        }
    );
}

void TcpConnection::asyncReadBody(uint32_t body_size) {
    read_body_buffer_.resize(body_size);
    
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(read_body_buffer_, body_size),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            handleReadBody(ec);
        }
    );
}

void TcpConnection::asyncWrite() {
    auto& message = write_queue_.front();
    
    // Prepare a vector of buffers for the gather write operation
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(message.header_buffer));
    
    if (!message.body.empty()) {
        buffers.push_back(boost::asio::buffer(message.body));
    }
    
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_,
        buffers,
        [this, self](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
            handleWrite(ec);
        }
    );
}

void TcpConnection::handleReadHeader(const boost::system::error_code& error) {
    if (error) {
        handleError(error);
        return;
    }
    
    // Decode the header
    current_header_.decodeFromBuffer(read_header_buffer_);
    
    // Validate the header
    if (!current_header_.isValid()) {
        // Invalid header, close connection
        stop();
        return;
    }
    
    // Read the message body
    uint32_t body_size = current_header_.getBodySize();
    if (body_size > 0) {
        asyncReadBody(body_size);
    } else {
        // Empty message body, notify callback with an empty vector
        if (message_callback_) {
            message_callback_(std::vector<char>(), 
                             current_header_.getMessageType(), 
                             current_header_.getFlags());
        }
        
        // Start reading the next message
        asyncReadHeader();
    }
}

void TcpConnection::handleReadBody(const boost::system::error_code& error) {
    if (error) {
        handleError(error);
        return;
    }
    
    // Notify the callback
    if (message_callback_) {
        message_callback_(read_body_buffer_, 
                         current_header_.getMessageType(), 
                         current_header_.getFlags());
    }
    
    // Start reading the next message
    asyncReadHeader();
}

void TcpConnection::handleWrite(const boost::system::error_code& error) {
    if (error) {
        handleError(error);
        return;
    }
    
    // Remove the message from the queue
    {
        std::lock_guard<std::mutex> lock(write_mutex_);
        write_queue_.pop_front();
        
        if (!write_queue_.empty()) {
            // More messages in the queue, continue writing
            asyncWrite();
        } else {
            // No more messages to write
            write_in_progress_ = false;
        }
    }
}

void TcpConnection::handleError(const boost::system::error_code& error) {
    if (error == boost::asio::error::eof ||
        error == boost::asio::error::connection_reset) {
        // Connection closed cleanly by peer or reset
        if (error_callback_) {
            error_callback_(error);
        }
        stop();
    } else {
        // Some other error
        if (error_callback_) {
            error_callback_(error);
        }
        // For some errors, we might want to continue rather than stopping,
        // but for simplicity, we stop on all errors for now
        stop();
    }
}

} // namespace chat_app