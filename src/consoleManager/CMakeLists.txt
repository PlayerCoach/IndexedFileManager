add_library(ConsoleManagerLib
    consoleManager.cpp
)

target_include_directories(ConsoleManagerLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(ConsoleManagerLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/commands
)
target_link_libraries(ConsoleManagerLib DataEntryLib)