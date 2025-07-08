cmake_minimum_required(VERSION 3.10)
project(ChatApplication LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(Boost 1.70 REQUIRED COMPONENTS system thread)
find_package(SQLite3 REQUIRED)
find_package(nlohmann_json 3.9 REQUIRED)
find_package(Curses REQUIRED)

# Include subdirectories
add_subdirectory(src/common)
add_subdirectory(src/client)
add_subdirectory(src/server)

# Client executable
add_executable(chat_client
    src/client/chat_client.cpp
    src/client/message_handler.cpp
    src/client/ui/console_ui.cpp
    src/client/main.cpp
)
target_link_libraries(chat_client
    PRIVATE
        chatapp_common
        Boost::system
        Boost::thread
        SQLite3::SQLite3
        ${CURSES_LIBRARIES}
        nlohmann_json::nlohmann_json
)
target_include_directories(chat_client
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Server executable
add_executable(chat_server
    src/server/chat_server.cpp
    src/server/session_manager.cpp
    src/server/message_router.cpp
    src/server/storage_manager.cpp
    src/server/main.cpp
)
target_link_libraries(chat_server
    PRIVATE
        chatapp_common
        Boost::system
        Boost::thread
        SQLite3::SQLite3
        nlohmann_json::nlohmann_json
)
target_include_directories(chat_server
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Enable testing
enable_testing()
find_package(GTest)
if(GTest_FOUND)
    add_executable(run_tests
        tests/test_message.cpp
        tests/test_session.cpp
        tests/test_database.cpp
    )
    target_link_libraries(run_tests
        PRIVATE
        chatapp_common
        GTest::GTest
        GTest::Main
    )
    add_test(NAME UnitTests COMMAND run_tests)
endif()