# Tests configuration
include(GoogleTest)

# Common tests
set(COMMON_TEST_SOURCES
    common_tests/message_test.cpp
    common_tests/protocol_test.cpp
    common_tests/crypto_test.cpp
+   common_tests/config_loader_test.cpp  # Add config loader test
)

# Client tests
set(CLIENT_TEST_SOURCES
    client_tests/chat_client_test.cpp
    client_tests/message_handler_test.cpp
)

# Server tests
set(SERVER_TEST_SOURCES
    server_tests/chat_server_test.cpp
    server_tests/session_manager_test.cpp
    server_tests/message_router_test.cpp
    server_tests/storage_manager_test.cpp
)

# Common tests
add_executable(common_tests ${COMMON_TEST_SOURCES})
target_link_libraries(common_tests
    PRIVATE
        chat_common
        gtest
        gtest_main
        gmock
)
gtest_discover_tests(common_tests)