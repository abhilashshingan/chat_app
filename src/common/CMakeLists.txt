# Common library shared between client and server

# Source files for the common library
set(COMMON_SOURCES
    message.cpp
    chat_message.cpp
    protocol.cpp
    tcp_connection.cpp
    crypto.cpp
    config.cpp
)

# Create static library
add_library(chatapp_common STATIC ${COMMON_SOURCES})

# Link with Boost libraries
find_package(Boost 1.70 REQUIRED COMPONENTS system thread)
target_link_libraries(chatapp_common
    PUBLIC
        Boost::system
        Boost::thread
)

# Link with OpenSSL if available
find_package(OpenSSL)
if(OpenSSL_FOUND)
    target_link_libraries(chatapp_common
        PRIVATE
            OpenSSL::SSL
            OpenSSL::Crypto
    )
endif()

# Link with nlohmann_json
find_package(nlohmann_json 3.9 REQUIRED)
target_link_libraries(chatapp_common
    PRIVATE
        nlohmann_json::nlohmann_json
)

# Include directories
target_include_directories(chatapp_common
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# Set compile definitions
target_compile_definitions(chatapp_common 
    PRIVATE 
        BOOST_ASIO_NO_DEPRECATED
)