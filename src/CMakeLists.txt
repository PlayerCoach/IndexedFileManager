add_executable(Main
main/main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/const)

target_link_libraries(Main ConsoleManagerLib)
