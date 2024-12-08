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
#include <optional>
#include "bTreeEntry.hpp"

class IndexManager 
{
    private:
    std::string indexFilePath;
    Node rootCache;
    FileManager IndexFileManager;
    const int treeOrder = 3;
    const int indexPageSize = Node::size(treeOrder);
    uint32_t writeBlockIndex = 0;

    public:
    IndexManager() = default;
    IndexManager(std::string indexFilePath);

    void insert(DataEntry dataEntry, uint32_t databaseBlockIndex);
    std::optional<size_t> getInsertPosition(Node& node, uint64_t key);
    Node findLeafNodeForKey(uint64_t key);
    Node getNode(uint32_t blockIndex);
    Node createNode(bool isLeaf, uint32_t blockIndex);
    void readBTree();
    void split(DataEntry& data, Node& node, uint64_t key, uint32_t dataBlockPtr);
    void splitRoot(DataEntry& data, Node& node, uint64_t key, uint32_t dataBlockPtr);
    //void checkIfCanCompensate(Node& node, uint64_t key, uint32_t dataBlockPtr);
};