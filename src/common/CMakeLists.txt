# Common library shared between client and server

# Common library shared between client and server

set(COMMON_SOURCES
    message.cpp
    protocol.cpp
    crypto.cpp
    config.cpp
   config_loader.cpp  # Add the new source file
)

add_library(chat_common STATIC ${COMMON_SOURCES})

target_link_libraries(chat_common
    PRIVATE
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        ${CMAKE_THREAD_LIBS_INIT}
)

 # For C++17 filesystem support on GCC
 if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
     target_link_libraries(chat_common PRIVATE stdc++fs)
 endif()

target_include_directories(chat_common
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)