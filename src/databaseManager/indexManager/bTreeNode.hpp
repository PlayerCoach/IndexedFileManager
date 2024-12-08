#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include "bTreeEntry.hpp"

class Node {
private:
    uint32_t order;
    uint32_t selfPtr;
    std::optional<uint32_t> parentPtr;
    bool isLeaf;
    bool isFull;
    uint32_t numberOfKeys;
    std::vector<BTreeEntry> entries;

public:
    Node() = default;
    
    Node(uint32_t order, uint32_t selfPtr,  std::optional<uint32_t> parentPtr, bool isLeaf);

    Node(uint32_t order, uint32_t selfPtr, std::optional<uint32_t> parentPtr, bool isLeaf, bool isFull, uint32_t numberOfKeys,
            std::vector<BTreeEntry> entries)
        : order(order), selfPtr(selfPtr), parentPtr(parentPtr), isLeaf(isLeaf), isFull(isFull), numberOfKeys(numberOfKeys),
            entries(entries){}


    static int size(int order);

    void insertKey(uint64_t key, uint32_t dataBlockPtr);
    void insertChildPtr(uint32_t childPtr, size_t position);
    void updateChildPtrs(size_t position);

    std::vector<BTreeEntry> getEntries() const { return entries; }
    void setEntries(std::vector<BTreeEntry> entries) { this->entries = entries; }

    size_t getMaxNumberOfKeys() { return 2 * order; }
    bool getIsLeaf() { return isLeaf; }
    bool getIsFull() { return isFull; }
    uint32_t getBlockIndex() { return selfPtr; }
    std::optional<int32_t> getParentPtr() { return parentPtr; }
    uint32_t getNumberOfKeys() { return numberOfKeys; }


    std::unique_ptr<char[]> serialize();
    static std::optional<Node> deserialize(char* data, uint32_t order);
};