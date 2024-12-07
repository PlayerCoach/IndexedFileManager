#include "bTreeNode.hpp"

int Node::size(int order) {
    int size = 0;
    size += sizeof(uint32_t); // parentPtr
    size += sizeof(bool); // isFull
    size += sizeof(bool); // isLeaf
    size += sizeof(uint32_t); // numberOfKeys
    size += 2 * order * (sizeof(uint64_t) + sizeof(uint32_t)); // keyDataPairs
    size += (2 * order + 1) * sizeof(int32_t); // childPtrs
    return size;
}

void Node::insertKey(uint64_t key, uint32_t dataBlockPtr, size_t position) {
    keyDataPairs.insert(keyDataPairs.begin() + position, {key, dataBlockPtr});
    numberOfKeys++;
    if(numberOfKeys == 2 * order) {
        isFull = true;
    }
 
}

void Node::insertChildPtr(uint32_t childPtr, size_t position) {
    childPtrs.insert(childPtrs.begin() + position, childPtr);

}

std::unique_ptr<char[]> Node::serialize() {
    size_t nodeSize = size(order);
    std::unique_ptr<char[]> data = std::make_unique<char[]>(nodeSize);
    char* ptr = data.get();

    // Serialize node metadata
    memcpy(ptr, &parentPtr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    memcpy(ptr, &isFull, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &isLeaf, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &numberOfKeys, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

     // Serialize childPtrs
    for (size_t i = 0; i < 2 * order + 1; ++i) {
        if (i < childPtrs.size()) {
            memcpy(ptr, &childPtrs[i], sizeof(int32_t));
        } else {
            int32_t invalidChildPtr = -1;
            memcpy(ptr, &invalidChildPtr, sizeof(uint32_t));
        }
        ptr += sizeof(uint32_t);
    }

    // Serialize keys and dataBlockPtrs
    for (size_t i = 0; i < 2 * order; ++i) {
        if (i < keyDataPairs.size()) {
            memcpy(ptr, &keyDataPairs[i].first, sizeof(uint64_t));
            ptr += sizeof(uint64_t);
            memcpy(ptr, &keyDataPairs[i].second, sizeof(uint32_t));
            ptr += sizeof(uint32_t);
        } else {
            uint64_t invalidKey = static_cast<uint64_t>(-1);
            uint32_t invalidDataPtr = static_cast<uint32_t>(-1);
            memcpy(ptr, &invalidKey, sizeof(uint64_t));
            ptr += sizeof(uint64_t);
            memcpy(ptr, &invalidDataPtr, sizeof(uint32_t));
            ptr += sizeof(uint32_t);
        }
    }

    return data;
}

std::optional<Node> Node::deserialize(char* data, uint32_t order) {
    char* ptr = data;
    uint32_t parentPtr = *reinterpret_cast<uint32_t*>(ptr);
    ptr += sizeof(uint32_t);
    bool isFull = *reinterpret_cast<bool*>(ptr);
    ptr += sizeof(bool);
    bool isLeaf = *reinterpret_cast<bool*>(ptr);
    ptr += sizeof(bool);
    uint32_t numberOfKeys = *reinterpret_cast<uint32_t*>(ptr);
    ptr += sizeof(uint32_t);

    //read childptrs with -1 so that we can assign ptr to the correct key
    std::vector<int32_t> childPtrs;
    for (size_t i = 0; i < 2 * order + 1; ++i) {
        int32_t childPtr = *reinterpret_cast<int32_t*>(ptr);
        ptr += sizeof(int32_t);
        if (childPtr == -1) {
            break;
        }
        childPtrs.push_back(childPtr);
    }

    // read order number of keys and dataBlockPtrs
    std::vector<std::pair<uint64_t, uint32_t>> keyDataPairs;
    for (auto i = 0; i < numberOfKeys; i++) {
        uint64_t key = *reinterpret_cast<uint64_t*>(ptr);
        ptr += sizeof(uint64_t);
        uint32_t dataBlockPtr = *reinterpret_cast<uint32_t*>(ptr);
        ptr += sizeof(uint32_t);
        keyDataPairs.push_back({key, dataBlockPtr});
    }

    return Node(order, parentPtr, isLeaf, isFull, numberOfKeys, keyDataPairs, childPtrs);
}
