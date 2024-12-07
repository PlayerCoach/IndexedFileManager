#include <iostream>
#include <string>
#include <memory>
#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "fileIOManager.hpp"
#include "dataEntry.hpp"
#include "bTreeNode.hpp"

class IndexManager 
{
    private:
    std::string indexFilePath;
    Node rootCache;
    FileManager IndexFileManager;
    const int treeOrder = 3;
    const int indexPageSize = Node::size(treeOrder);

    public:
    IndexManager() = default;
    IndexManager(std::string indexFilePath);
};