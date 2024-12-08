#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include "bTreeEntry.hpp"
#include "serializeOptional.hpp"


class Node 
{

private:

    uint32_t order;
    uint32_t selfPtr;
    std::optional<uint32_t> parentPtr;
    bool isLeaf;
    bool isFull;
    uint32_t numberOfKeys;
    bool entryWithoutKeyExists = false;
    std::vector<BTreeEntry> entries;

public:

    Node() = default;
    
    Node(uint32_t order, uint32_t selfPtr,  std::optional<uint32_t> parentPtr, bool isLeaf);

    Node(uint32_t order, uint32_t selfPtr, std::optional<uint32_t> parentPtr, bool isLeaf, bool isFull, std::vector<BTreeEntry> entries);
        

    void countKeys();
    void clearNode();

    void insertKey(uint64_t key, uint32_t dataBlockPtr);
    void insertChildPtr(std::optional<uint32_t> childPtr);
    void insertEntry(const BTreeEntry& entry);

    void setEntries(std::vector<BTreeEntry> entries);
    void setParentPtr(std::optional<uint32_t> parentPtr);
    void setIsLeaf(bool isLeaf);
    void setIsFull(bool isFull);
    void setEntryChildPtr(uint64_t key_value, uint32_t childPtr);

    void deleteEntryAtIndex(size_t index);
    BTreeEntry popLeftMostEntryWithKey();
    BTreeEntry popRightMostEntryWithKey();
    std::optional<BTreeEntry> popEntryWithoutKey();

    std::vector<BTreeEntry> getEntries() const { return entries; }
    size_t getMaxNumberOfKeys() const;
    bool getIsLeaf() const ;
    bool getIsFull() const ;
    uint32_t getBlockIndex() const ;
    std::optional<uint32_t> getParentPtr() const;
    uint32_t getNumberOfKeys() const;
    BTreeEntry retrieveMedianKeyEntry() const;

    std::pair<std::vector<BTreeEntry>, std::vector<BTreeEntry>> splitNode();

    static int size(int order);
    std::unique_ptr<char[]> serialize() const;
    static std::optional<Node> deserialize(char* data, uint32_t order);
};