add_executable(Main
main/main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/const)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/consoleManager)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dataEntry)

target_link_libraries(Main ConsoleManagerLib)
target_link_libraries(Main DataEntryLib)
