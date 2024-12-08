#include "bTreeNode.hpp"

Node::Node(uint32_t order, uint32_t selfPtr, std::optional<uint32_t> parentPtr, bool isLeaf)
    : order(order), selfPtr(selfPtr), parentPtr(parentPtr), isLeaf(isLeaf), isFull(false), numberOfKeys(0) {  
    entries.reserve(2 * order + 1);
   
}


int Node::size(int order) {
    int size = 0;
    size += sizeof(uint32_t); // parentPtr
    size += sizeof(bool); // is selfPtrSet
    size += sizeof(uint32_t); // selfPtr
    size += sizeof(bool); // isFull
    size += sizeof(bool); // isLeaf
    size += sizeof(uint32_t); // numberOfKeys
    size += (2 * order + 1) * BTreeEntry::size();
    
    return size;
}

void Node::insertKey(uint64_t key, uint32_t dataBlockPtr) {
    // Find the position to insert
    size_t position = 0;
    while (position < numberOfKeys) {
        if (entries[position].getKey().value() > key) {
            break;
        }
        position++;
    }

    // Create the new entry to insert
    BTreeEntry newEntry(key, dataBlockPtr, std::nullopt);
    
    // Insert the new entry at the correct position
    entries.insert(entries.begin() + position, newEntry);
    numberOfKeys++;

    // Check if the node is full
    if (numberOfKeys == 2 * order) {
        isFull = true;
    }

    // If the node is not a leaf, update child pointers
    if (!isLeaf) {
        updateChildPtrs(position);
    }

}

void Node::insertChildPtr(uint32_t childPtr, size_t position) {
    entries[position].setChildPtr(childPtr);
    if (!isLeaf) {
        updateChildPtrs(position);
    }

}

std::unique_ptr<char[]> Node::serialize() {
    size_t nodeSize = size(order);
    std::unique_ptr<char[]> data = std::make_unique<char[]>(nodeSize);
    char* ptr = data.get();

    // Serialize node metadata
    memcpy(ptr, &selfPtr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    if(parentPtr.has_value()) {
        bool parentPtrExists = true;
        memcpy(ptr, &parentPtr, sizeof(bool));
        ptr += sizeof(bool);
        memcpy(ptr, &parentPtr.value(), sizeof(uint32_t));
    } else {
        bool parentPtrExists = false;
        memcpy(ptr, &parentPtrExists, sizeof(bool));
        ptr += sizeof(bool);
        uint32_t null = 0;
        memcpy(ptr, &null, sizeof(uint32_t));
    }
    ptr += sizeof(uint32_t);

    memcpy(ptr, &isFull, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &isLeaf, sizeof(bool));
    ptr += sizeof(bool);
    memcpy(ptr, &numberOfKeys, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    // serialize non empty entires
    size_t nonEmptyEntriesCounter = 0;
    for(auto& entry : entries) {
        if (!entry.checkIfEntryIsEmtpy()) {
            nonEmptyEntriesCounter++;
            std::unique_ptr<char[]> entryData = entry.serialize();
            memcpy(ptr, entryData.get(), BTreeEntry::size());
            ptr += BTreeEntry::size();
        }
    }

    // fill the rest of the entries with empty entries

    for (size_t i = nonEmptyEntriesCounter; i < 2 * order + 1; ++i) {
        BTreeEntry emptyEntry;
        std::unique_ptr<char[]> entryData = emptyEntry.serialize();
        memcpy(ptr, entryData.get(), BTreeEntry::size());
        ptr += BTreeEntry::size();
    }

    return data;
}

std::optional<Node> Node::deserialize(char* data, uint32_t order) {

    char* ptr = data;
    uint32_t selfPtr = *reinterpret_cast<int32_t*>(ptr);
    ptr += sizeof(uint32_t);
    bool parentPtrExists = *reinterpret_cast<bool*>(ptr);
    ptr += sizeof(bool);
    std::optional<uint32_t> parentPtr;
    if (parentPtrExists) {
        parentPtr = *reinterpret_cast<uint32_t*>(ptr);
    }
    ptr += sizeof(uint32_t);
    bool isFull = *reinterpret_cast<bool*>(ptr);
    ptr += sizeof(bool);
    bool isLeaf = *reinterpret_cast<bool*>(ptr);
    ptr += sizeof(bool);
    uint32_t numberOfKeys = *reinterpret_cast<uint32_t*>(ptr);
    ptr += sizeof(uint32_t);

    // Deserialize entries
    std::vector<BTreeEntry> entries;

    for (size_t i = 0; i < 2 * order + 1; ++i) {
        std::optional<BTreeEntry> entry = BTreeEntry::deserialize(ptr);
        if (!entry.has_value()) {
            return std::nullopt;
        }
        if(entry.value().checkIfEntryIsEmtpy()) {
            break;
        }
        entries.push_back(entry.value());
        ptr += BTreeEntry::size();
    }

    return Node(order,selfPtr, parentPtr, isLeaf, isFull, numberOfKeys, entries);
}

void Node::updateChildPtrs(size_t position) {
    if (position == 0) {
        entries[position].setChildPtr(entries[position + 1].getChildPtr().value());
    } else if (position == numberOfKeys) {
        entries[position].setChildPtr(entries[position - 1].getChildPtr().value());
    } else {
        entries[position].setChildPtr(entries[position + 1].getChildPtr().value());
        entries[position + 1].setChildPtr(entries[position - 1].getChildPtr().value());
    }
}