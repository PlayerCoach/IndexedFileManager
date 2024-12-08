#include "bTreeNode.hpp"

Node::Node(uint32_t order, uint32_t selfPtr, std::optional<uint32_t> parentPtr, bool isLeaf)
    : order(order), selfPtr(selfPtr), parentPtr(parentPtr), isLeaf(isLeaf), isFull(false), numberOfKeys(0) 
{  
    entries.reserve(2 * order + 1);
}


Node::Node(uint32_t order, uint32_t selfPtr, std::optional<uint32_t> parentPtr, bool isLeaf, bool isFull, std::vector<BTreeEntry> entries)
    : order(order), selfPtr(selfPtr), parentPtr(parentPtr), isLeaf(isLeaf), isFull(isFull), numberOfKeys(0), entries(entries) 
{
    countKeys();
}

void Node::countKeys() 
{
    numberOfKeys = 0;
    for(const auto& entry : entries) 
    {
        if(entry.getKey().has_value()) 
            numberOfKeys++;
    }
}

void Node::clearNode() // maby fix this later
{
    *this = Node(order, selfPtr, std::nullopt, false);
    
}


void Node::insertKey(uint64_t key, uint32_t dataBlockPtr) 
{
    if(isFull) 
        throw std::runtime_error("Node is full");

    auto position = std::find_if(entries.begin(), entries.end(), [&](const BTreeEntry& entry) {
        return !entry.getKey() || entry.getKey().value() > key;
    });

    entries.insert(position, BTreeEntry(key, dataBlockPtr, std::nullopt));
    numberOfKeys++;

    if (numberOfKeys == 2 * order) 
        isFull = true;
}

void Node::insertChildPtr(std::optional<uint32_t> childPtr) 
{
    if(childPtr.has_value()) {
        entries.insert(entries.begin(), BTreeEntry(std::nullopt, std::nullopt, childPtr));
    }
}

void Node::insertEntry(const BTreeEntry& entry) 
{
    if(this->entries.size() == 2 * order + 1)
        throw std::runtime_error("Node is full");

    auto position = std::find_if(entries.begin(), entries.end(), [&](const BTreeEntry& e) {
        return !e.getKey() || e.getKey().value() > entry.getKey().value();
    });

    entries.insert(position, entry);
    if(entry.getKey().has_value())
        numberOfKeys++;

    if (numberOfKeys == 2 * order) 
        isFull = true;
}


void Node::setEntries(std::vector<BTreeEntry> entries) 
{
    this->entries = entries;
    countKeys();
}

void Node::setParentPtr(std::optional<uint32_t> parentPtr) 
{
    this->parentPtr = parentPtr;
}

void Node::setIsLeaf(bool isLeaf) 
{
    this->isLeaf = isLeaf;
}

void Node::setIsFull(bool isFull) 
{
    this->isFull = isFull;
}

void Node::setNumberOfKeys(uint32_t numberOfKeys) 
{
    this->numberOfKeys = numberOfKeys;
}

void Node::setEntryChildPtr(uint64_t key_value, uint32_t childPtr) 
{
    auto entry = std::find_if(entries.begin(), entries.end(), [&](const BTreeEntry& entry) {
        return entry.getKey().value() == key_value;
    });

    if(entry != entries.end()) 
    {
        throw std::runtime_error("Entry not found");
        //entry->setChildPtr(childPtr);
    }
}


size_t Node::getMaxNumberOfKeys() const 
{
    return 2 * order;
}

bool Node::getIsLeaf() const 
{
    return isLeaf;
}

bool Node::getIsFull() const 
{
    return isFull;
}

uint32_t Node::getBlockIndex() const 
{
    return selfPtr;
}

std::optional<uint32_t> Node::getParentPtr() const 
{
    return parentPtr;
}

uint32_t Node::getNumberOfKeys() const 
{
    return numberOfKeys;
}


int Node::size(int order) 
{
    int size = 0;

    size += sizeof(uint32_t); // selfPtr
    size += sizeOptional<uint32_t>(); // parentPtr
    size += sizeof(bool); // isFull
    size += sizeof(bool); // isLeaf
    size += (2 * order + 1) * BTreeEntry::size();
    
    return size;
}

std::unique_ptr<char[]> Node::serialize() const
{
    size_t nodeSize = size(order);
    std::unique_ptr<char[]> data = std::make_unique<char[]>(nodeSize);
    char* ptr = data.get();

    memcpy(ptr, &selfPtr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    serializeOptional<uint32_t>(parentPtr, ptr);
    
    memcpy(ptr, &isFull, sizeof(bool));
    ptr += sizeof(bool);

    memcpy(ptr, &isLeaf, sizeof(bool));
    ptr += sizeof(bool);

    // serialize non empty entires
    size_t writtenEntries = 0;
    for(const auto& entry : entries) 
    {
        if (!entry.checkIfEntryIsEmtpy())
        {
            std::unique_ptr<char[]> entryData = entry.serialize();
            memcpy(ptr, entryData.get(), BTreeEntry::size());
            ptr += BTreeEntry::size();
            writtenEntries++;
        }
    }

    // fill the rest of the entries with empty entries

    for (size_t i = writtenEntries; i < 2 * order + 1; ++i) 
    {
        BTreeEntry emptyEntry;
        std::unique_ptr<char[]> entryData = emptyEntry.serialize();
        memcpy(ptr, entryData.get(), BTreeEntry::size());
        ptr += BTreeEntry::size();
    }

    return data;
}

std::optional<Node> Node::deserialize(char* data, uint32_t order) 
{
    char* ptr = data;

    uint32_t selfPtr = *reinterpret_cast<int32_t*>(ptr);
    ptr += sizeof(uint32_t);

    std::optional<uint32_t> parentPtr = deserializeOptional<uint32_t>(ptr);

    bool isFull = *reinterpret_cast<bool*>(ptr);
    ptr += sizeof(bool);

    bool isLeaf = *reinterpret_cast<bool*>(ptr);
    ptr += sizeof(bool);

    // Deserialize entries
    std::vector<BTreeEntry> entries;

    for (size_t i = 0; i < 2 * order + 1; ++i) 
    {
        auto entry = BTreeEntry::deserialize(ptr);

        if(entry.value().checkIfEntryIsEmtpy()) 
        {
            ptr += BTreeEntry::size();
            continue;
        }
        entries.push_back(entry.value());
        ptr += BTreeEntry::size();
    }

    return Node(order,selfPtr, parentPtr, isLeaf, isFull, entries);
}

