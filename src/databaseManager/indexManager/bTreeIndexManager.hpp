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
#include <queue>

class IndexManager 
{
    private:
    std::string indexFilePath;
    Node rootCache;
    FileManager IndexFileManager;
    const uint32_t treeOrder = 2;
    const int indexPageSize = Node::size(treeOrder);
    uint32_t writeBlockIndex = 0;

    public:
    IndexManager() = default;
    IndexManager(std::string indexFilePath);
    void insertPreparation(DataEntry dataEntry, uint32_t databaseBlockIndex);
    void insertToLeaf(BTreeEntry entry);
    void insertToNode(Node& node, BTreeEntry entry);
    std::optional<size_t> getInsertPosition(Node& node, uint64_t key);
    Node findLeafNodeForKey(uint64_t key);
    Node getNode(uint32_t blockIndex);
    Node createNode(bool isLeaf, uint32_t blockIndex);
    void readBTree();
    void split(Node& node, BTreeEntry entry);
    void splitRoot(Node& node, BTreeEntry entry);
    bool checkIfCanCompensate(Node& node, BTreeEntry entry);
    std::pair<std::optional<Node>,std::optional<Node>> findSiblings(const Node& parentNode, uint32_t blockIndex);
    void compensate(Node& node, Node& parentNode, Node& siblingNode, BTreeEntry entry,  bool isLeftSibling);
    void readNode(Node& node);
};