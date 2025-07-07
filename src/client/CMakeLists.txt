# Client executable
set(CLIENT_SOURCES
    main.cpp
    chat_client.cpp
    message_handler.cpp
    ui/console_ui.cpp
)

# Optional GUI sources
option(BUILD_GUI_CLIENT "Build GUI client interface" OFF)
if(BUILD_GUI_CLIENT)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    list(APPEND CLIENT_SOURCES ui/gui_ui.cpp)
endif()

# Create client executable
add_executable(chat_client ${CLIENT_SOURCES})

# Link libraries
target_link_libraries(chat_client
    PRIVATE
        chat_common
        Boost::system
        ${CURSES_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

# GUI linking
if(BUILD_GUI_CLIENT)
    target_link_libraries(chat_client PRIVATE Qt5::Widgets)
endif()

# Set output directory
set_target_properties(chat_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation
install(TARGETS chat_client
        RUNTIME DESTINATION bin)
EOF

cd chat_app/src/client && touch main.cpp chat_client.cpp message_handler.cpp
cd ui && touch console_ui.cpp gui_ui.cpp
echo "Created client CMakeLists.txt and placeholder files"