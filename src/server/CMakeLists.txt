# Server executable
set(SERVER_SOURCES
    main.cpp
    chat_server.cpp
    session_manager.cpp
    message_router.cpp
    storage_manager.cpp
)

# Create server executable
add_executable(chat_server ${SERVER_SOURCES})

# Link libraries
target_link_libraries(chat_server
    PRIVATE
        chat_common
        ${SQLite3_LIBRARIES}
        Boost::system
        ${CMAKE_THREAD_LIBS_INIT}
)

# Set output directory
set_target_properties(chat_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation
install(TARGETS chat_server
        RUNTIME DESTINATION bin)
EOF
```cd chat_app/src/server && touch main.cpp chat_server.cpp session_manager.cpp message_router.cpp storage_manager.cpp
```
echo "Created server CMakeLists.txt and placeholder files"