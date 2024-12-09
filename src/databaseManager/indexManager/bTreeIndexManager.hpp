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
    const size_t indexPageSize = Node::size(treeOrder);
    uint32_t writeBlockIndex = 0;

    public:
    IndexManager() = default;
    IndexManager(std::string indexFilePath);
    void insertPreparation(DataEntry dataEntry, uint32_t databaseBlockIndex);
    void insertToLeaf(BTreeEntry entry);
    void insertToNode(Node& node, BTreeEntry entry);
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
    std::optional<Node> getParentNode(const Node& node);

    std::optional<Node> findNodeWithKey(uint64_t key);

    std::string deleteKeyPreparation(uint64_t key);
    void deleteKey(Node& node, uint64_t key);
    void deleteKeyFromLeaf(Node& node, uint64_t key);
        std::pair<std::optional<BTreeEntry>, std::optional<Node>> findMaxElementFromLeftSubtree(Node& node);
        std::pair<std::optional<BTreeEntry>, std::optional<Node>> findMinElementFromRightSubtree(Node& node);
    bool checkIfCanCompensateAfterDeletion(Node& node, uint64_t key);
    void compensateAfterDeletion(Node& node, Node& parentNode, Node& siblingNode, uint64_t key,  bool isLeftSibling);
};