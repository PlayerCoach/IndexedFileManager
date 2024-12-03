add_library(DataEntryLib
    dataEntry.cpp
    keyGen.cpp
    record.cpp
)

target_include_directories(DataEntryLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)


