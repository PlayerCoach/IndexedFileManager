add_executable(Main
main/main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/const)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dataEntry)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fileIOManager)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/databaseManager)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/consoleManager)

target_link_libraries(Main DataEntryLib)
target_link_libraries(Main ConsoleManagerLib)
target_link_libraries(Main FileIOManagerLib)
target_link_libraries(Main DatabaseManagerLib)

