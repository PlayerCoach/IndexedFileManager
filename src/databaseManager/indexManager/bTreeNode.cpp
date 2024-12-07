#include "bTreeNode.hpp"

int Node::size(int order) {
    int size = 0;
    size += sizeof(uint32_t); // parentPtr
    size += sizeof(bool); // isFull
    size += sizeof(bool); // isLeaf
    size += sizeof(uint32_t); // numberOfKeys
    size += 2 * order * (sizeof(uint64_t) + sizeof(uint32_t)); // keyDataPairs
    size += (2 * order + 1) * sizeof(uint32_t); // childPtrs
    return size;
}

void Node::insertKey(uint64_t key, uint32_t dataBlockPtr) {
    if(isFull) {
        throw std::runtime_error("Node is full");
    }
    auto it = std::lower_bound(keyDataPairs.begin(), keyDataPairs.end(), std::make_pair(key, dataBlockPtr),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    keyDataPairs.insert(it, {key, dataBlockPtr});
    numberOfKeys = keyDataPairs.size();
    isFull = (numberOfKeys >= 2 * order); // Mark full if node reaches capacity
}

void Node::insertChildPtr(uint32_t childPtr, uint64_t childPtrMaxKey) {
    if(isLeaf) {
        throw std::runtime_error("Cannot insert child pointer in leaf node");
    }
    //get index of first key greater than childPtrMaxKey
    auto it = std::lower_bound(keyDataPairs.begin(), keyDataPairs.end(), childPtrMaxKey,
        [](const auto& a, const auto& b) { return a.first < b; });
    size_t index = it - keyDataPairs.begin();
    childPtrs.insert(childPtrs.begin() + index, childPtr);

    if(childPtrs.size() > 2 * order + 1) {
        throw std::runtime_error("Child pointer size exceeds limit");
    }

    isFull = (childPtrs.size() >= 2 * order + 1); // Mark full if node reaches capacity
}

std::unique_ptr<char[]> Node::serialize() {
    size_t nodeSize = size(order);
    std::unique_ptr<char[]> data = std::make_unique<char[]>(nodeSize);
    char* ptr = data.get();

    memcpy(ptr, &parentPtr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    memcpy(ptr, &isFull, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &isLeaf, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &numberOfKeys, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    for(const auto& pair : keyDataPairs) {
        memcpy(ptr, &pair.first, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        memcpy(ptr, &pair.second, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
    }

    for(const auto& childPtr : childPtrs) {
        memcpy(ptr, &childPtr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
    }

    return data;
}

static std::optional<Node> deserialize(char* data, uint32_t order) {
    char* ptr = data;

    uint32_t parentPtr;
    memcpy(&parentPtr, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    bool isFull;
    memcpy(&isFull, ptr, sizeof(bool));
    ptr += sizeof(bool);

    bool isLeaf;
    memcpy(&isLeaf, ptr, sizeof(bool));
    ptr += sizeof(bool);

    uint32_t numberOfKeys;
    memcpy(&numberOfKeys, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    std::vector<std::pair<uint64_t, uint32_t>> keyDataPairs;

    for(uint32_t i = 0; i < numberOfKeys; i++) {
        uint64_t key;
        memcpy(&key, ptr, sizeof(uint64_t));
        ptr += sizeof(uint64_t);

        uint32_t dataBlockPtr;
        memcpy(&dataBlockPtr, ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        keyDataPairs.push_back({key, dataBlockPtr});
    }

    std::vector<uint32_t> childPtrs;

    if(!isLeaf) {
        for(uint32_t i = 0; i < numberOfKeys + 1; i++) {
            uint32_t childPtr;
            memcpy(&childPtr, ptr, sizeof(uint32_t));
            ptr += sizeof(uint32_t);

            childPtrs.push_back(childPtr);
        }
    }

    return Node(order, parentPtr, isLeaf, isFull, numberOfKeys, keyDataPairs, childPtrs);
    
}