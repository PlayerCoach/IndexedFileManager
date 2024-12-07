#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>

struct BTreeEntry
{
    std::optional<uint64_t> key;
    std::optional<uint32_t> dataBlockPtr;
    int32_t childPtr; // points to the child node of elements smaller than key
}

class Node {
private:
    uint32_t order;
    uint32_t selfPtr;
    int32_t parentPtr;
    bool isLeaf;
    bool isFull;
    uint32_t numberOfKeys;
    std::vector<int32_t> childPtrs;
    std::vector<std::pair<uint64_t, uint32_t>> keyDataPairs; // Pair of key and data block pointer

public:
    Node() = default;
    
    Node(uint32_t order, uint32_t selfPtr,  uint32_t parentPtr, bool isLeaf);

    Node(uint32_t order, uint32_t selfPtr, uint32_t parentPtr, bool isLeaf, bool isFull, uint32_t numberOfKeys,
            std::vector<std::pair<uint64_t, uint32_t>> keyDataPairs, std::vector<int32_t> childPtrs)
        : order(order), selfPtr(selfPtr), parentPtr(parentPtr), isLeaf(isLeaf), isFull(isFull), numberOfKeys(numberOfKeys),
            keyDataPairs(keyDataPairs), childPtrs(childPtrs) {}

    static int size(int order);

    void insertKey(uint64_t key, uint32_t dataBlockPtr, size_t position);
    void insertChildPtr(uint32_t childPtr, size_t position);
    void updateChildPtrs(size_t position);

    std::vector<std::pair<uint64_t, uint32_t>> getKeyDataPairs() const { return keyDataPairs; }
    void setKeyDataPairs(std::vector<std::pair<uint64_t, uint32_t>> keyDataPairs) { this->keyDataPairs = keyDataPairs; }
    std::vector<int32_t> getChildPtrs() const { return childPtrs; }

    size_t getMaxNumberOfKeys() { return 2 * order; }
    bool getIsLeaf() { return isLeaf; }
    bool getIsFull() { return isFull; }
    uint32_t getBlockIndex() { return selfPtr; }
    int32_t getParentPtr() { return parentPtr; }
    uint32_t getNumberOfKeys() { return numberOfKeys; }


    std::unique_ptr<char[]> serialize();
    static std::optional<Node> deserialize(char* data, uint32_t order);
};