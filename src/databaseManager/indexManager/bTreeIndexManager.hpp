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
    std::vector<uint32_t> freeBlocks;

    int treeHeight = 0;
    size_t cacheSize = treeHeight + 1;
    std::unordered_map<uint32_t, Node> cache;
    int cacheHits = 0;
    int cacheMisses = 0;

    public:

    IndexManager() = default;
    IndexManager(std::string indexFilePath);

    /* UTILITIES */

    Node createNode(bool isLeaf, uint32_t blockIndex);
    void deleteNode(uint32_t blockIndex);
    Node getNode(uint32_t blockIndex);
    void readNode(Node& node);
    void writeNodeToFile(uint32_t blockIndex, Node& node);
    void readBTree();
    std::pair<std::optional<Node>, bool> getNodeForKey(uint64_t key);

    /* COMPENSATE LOGIC */

    std::optional<Node> getParentNode(const Node& node);
    size_t findChildIndex(const Node& node, uint32_t childBlockIndex);
    std::pair<std::optional<Node>,std::optional<Node>> findSiblings(const Node& parentNode, uint32_t blockIndex);
    void compensateHelper(Node& node, Node& parentNode, Node& siblingNode,
        std::optional<BTreeEntry> entry, uint64_t key, bool hasLeftSibling, bool isDeletion);


    /* INSERT LOGIC*/
    std::string insertPreparation(DataEntry dataEntry, uint32_t databaseBlockIndex);
    void insertToLeaf(Node& nodeToInsert, BTreeEntry entry);
    void insertToNode(Node& node, BTreeEntry entry);
    void split(Node& node, BTreeEntry entry);
    void splitRoot(Node& node, BTreeEntry entry);
    bool checkIfCanCompensate(Node& node, BTreeEntry entry);
    void compensateAfterInsertion(Node& node, Node& parentNode, Node& siblingNode,
        BTreeEntry entry, bool hasLeftSibling);


    /* DELETE LOGIC */
    std::string deleteKeyPreparation(uint64_t key);
    void deleteKey(Node& node, uint64_t key);
        bool handleMaxElementFromLeftSubtree(Node& node, uint64_t key);
        bool handleMinElementFromRightSubtree(Node& node, uint64_t key);
    std::pair<std::optional<BTreeEntry>, std::optional<Node>> findMaxElementFromLeftSubtree(Node& node);
    std::pair<std::optional<BTreeEntry>, std::optional<Node>> findMinElementFromRightSubtree(Node& node);
    void handleKeyRemoval(Node& node, uint64_t key);
    bool checkIfCanCompensateAfterDeletion(Node& node, uint64_t key);
    void compensateAfterDeletion(Node& node, Node& parentNode, Node& siblingNode, uint64_t key, bool hasLeftSibling);
    void merge(Node& node, uint64_t key);
        void mergeWithLeftSibling(Node& node, Node& parentNode, uint64_t key, size_t index);
        void mergeWithRightSibling(Node& node, Node& parentNode, uint64_t key, size_t index);
        void updateTreeAfterMerge(Node& target, Node& parentNode, BTreeEntry& entryToDescend);
    
    /* SEARCH LOGIC */
    std::optional<uint32_t> search(uint64_t key);

    /* UPDATE LOGIC*/
    std::string updateKeyBlockPtr(uint64_t key, uint32_t newBlockPtr);

    /* CACHE LOGIC */
   
   

 

  
};