# Chat server executable

# Source files for the server
set(SERVER_SOURCES
    chat_server.cpp
    session_manager.cpp
    message_router.cpp
    storage_manager.cpp
    main.cpp
)

# Create server executable
add_executable(chat_server ${SERVER_SOURCES})

# Find and link dependencies
find_package(Boost 1.70 REQUIRED COMPONENTS system thread)
find_package(nlohmann_json 3.9 REQUIRED)
find_package(SQLite3 REQUIRED)

target_link_libraries(chat_server
    PRIVATE
        chatapp_common
        Boost::system
        Boost::thread
        nlohmann_json::nlohmann_json
        SQLite3::SQLite3
)

# Include directories
target_include_directories(chat_server
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Set output directory
set_target_properties(chat_server
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation
install(TARGETS chat_server
    RUNTIME DESTINATION bin
)