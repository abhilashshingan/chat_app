
# chat_app
=======
# C++ Real-Time Chat Application

A modern, multi-threaded chat application implemented in C++17 that enables real-time communication between multiple users through a client-server architecture.

## Features

- **Real-time messaging** with low latency communication
- **User authentication** with secure password handling
- **One-on-one chat** between users
- **Group chat rooms** with dynamic joining/leaving
- **Message persistence** via SQLite database
- **Online/offline status** indicators
- **Cross-platform** compatibility (Linux, macOS, Windows)
- **Console UI** with ncurses for a clean terminal interface
- **Optional GUI** support (Qt-based)

## Quick Start

### Building from Source

```bash
# Clone repository
git clone https://github.com/yourusername/cpp-chat-app.git
cd cpp-chat-app

# Use the setup script (Linux)
./setup_build.sh

# Or build manually
mkdir build && cd build
cmake ..
cmake --build .
```

### Running the Application

Start the server:
```bash
./bin/chat_server 8080
```

Connect with a client:
```bash
./bin/chat_client localhost 8080
```

## Client Commands

Once connected:
- `/login username password` - Log in to the server
- `/msg username message` - Send a private message
- `/join room` - Join a group chat room
- `/leave room` - Leave a group chat room
- `/list` - List online users
- `/quit` - Disconnect from server

## Project Structure

- `src/client/` - Client application
- `src/server/` - Server application
- `src/common/` - Shared components
- `include/` - Header files
- `tests/` - Unit and integration tests

## Dependencies

- C++17 compatible compiler
- CMake 3.14+
- Boost 1.70+ (ASIO)
- SQLite3
- ncurses
- OpenSSL
- nlohmann/json

## Contributing

We welcome contributions! See our [Contributing Guidelines](CONTRIBUTING.md) for more information.

