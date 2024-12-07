add_library(DatabaseManagerLib 
    DatabaseManager.cpp
    indexManager/bTreeIndexManager.cpp
    indexManager/bTreeNode.cpp
)

target_include_directories(DatabaseManagerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(DatabaseManagerLib DataEntryLib)
target_link_libraries(DatabaseManagerLib FileIOManagerLib)

