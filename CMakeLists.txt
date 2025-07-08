cmake_minimum_required(VERSION 3.14)
project(ChatApp VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_CLIENT "Build chat client" ON)
option(BUILD_SERVER "Build chat server" ON)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build example programs" ON)

# For JSON support
include(FetchContent)

# Fetch JSON library with specific configuration to avoid issues
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
# Prevent auto-update and add JSON as a system include to avoid warnings
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders ON CACHE INTERNAL "")
set(JSON_SystemInclude ON CACHE INTERNAL "")
FetchContent_MakeAvailable(json)

# Find Boost package
find_package(Boost 1.70 COMPONENTS system thread REQUIRED)
message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")

# Find OpenSSL (optional)
find_package(OpenSSL QUIET)
if(OpenSSL_FOUND)
    message(STATUS "Found OpenSSL: ${OPENSSL_INCLUDE_DIR}")
    add_definitions(-DWITH_OPENSSL)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# Build options
option(BUILD_CLIENT "Build chat client application" ON)
option(BUILD_SERVER "Build chat server application" ON)
option(BUILD_TESTS "Build test cases" OFF)

# Find dependencies
find_package(Threads REQUIRED)

# SQLite3
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

# Ncurses for client console UI
if(BUILD_CLIENT)
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
endif()

# For JSON support
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Find Boost for asynchronous networking
find_package(Boost 1.70 COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# OpenSSL for secure communication
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(src/common)

if(BUILD_SERVER)
    add_subdirectory(src/server)
endif()

if(BUILD_CLIENT)
    add_subdirectory(src/client)
endif()

if(BUILD_TESTS)
    enable_testing()
    # Include Google Test
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(tests)
endif()

 # Install configuration files
 install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config
         DESTINATION ${CMAKE_INSTALL_PREFIX}
         FILES_MATCHING PATTERN "*.env")
 
 # Create directories needed at runtime
 add_custom_target(create_runtime_directories ALL
     COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/config
     COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data
     COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/logs
     COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/certs
 )
 
 # Copy config files to the build directory
 add_custom_command(
     TARGET create_runtime_directories
     POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_directory
             ${CMAKE_CURRENT_SOURCE_DIR}/config
             ${CMAKE_BINARY_DIR}/config
 )