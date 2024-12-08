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
    void insertChildPtr(std::optional<uint32_t> childPtr);
    std::vector<BTreeEntry> getEntries() const { return entries; }
    void setEntries(std::vector<BTreeEntry> entries) { this->entries = entries; }
    void setParentPtr(std::optional<uint32_t> parentPtr) { this->parentPtr = parentPtr; } 
    void setIsLeaf(bool isLeaf) { this->isLeaf = isLeaf; }
    void setIsFull(bool isFull) { this->isFull = isFull; }
    void setNumberOfKeys(uint32_t numberOfKeys) { this->numberOfKeys = numberOfKeys; }
    void setEntryChildPtr(size_t index, uint32_t childPtr) { entries[index].setChildPtr(childPtr); }
    void insertEntry(const BTreeEntry& entry);
    size_t getMaxNumberOfKeys() { return 2 * order; }
    bool getIsLeaf() { return isLeaf; }
    bool getIsFull() { return isFull; }
    uint32_t getBlockIndex() { return selfPtr; }
    std::optional<int32_t> getParentPtr() { return parentPtr; }
    uint32_t getNumberOfKeys() { return numberOfKeys; }
    void clearNode();


    std::unique_ptr<char[]> serialize();
    static std::optional<Node> deserialize(char* data, uint32_t order);
};