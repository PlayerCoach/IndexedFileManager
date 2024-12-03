add_library(ConsoleManagerLib
    consoleManager.cpp
)

target_include_directories(ConsoleManagerLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
