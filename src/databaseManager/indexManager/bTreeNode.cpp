#include "bTreeNode.hpp"

Node::Node(uint32_t order, uint32_t selfPtr, bool isLeaf)
    : order(order), selfPtr(selfPtr), isLeaf(isLeaf), isFull(false), numberOfKeys(0) 
{  
    entries.reserve(2 * order + 1);
}


Node::Node(uint32_t order, uint32_t selfPtr, bool isLeaf, bool isFull, std::vector<BTreeEntry> entries)
    : order(order), selfPtr(selfPtr), isLeaf(isLeaf), isFull(isFull), numberOfKeys(0), entries(entries) 
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
        
        if(!entry.getKey().has_value() && entry.getChildPtr().has_value()) 
        {
            if(entryWithoutKeyExists) 
                throw std::runtime_error("Entry without key already exists");
            entryWithoutKeyExists = true;
        }

    }
}

void Node::clearNode() // maby fix this later
{
    *this = Node(order, selfPtr, false);
    
}


void Node::insertKey(uint64_t key, uint32_t dataBlockPtr) 
{
    // if(isFull)   < -- user should check if node is full
    //     throw std::runtime_error("Node is full");

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
    if(this->entryWithoutKeyExists) 
        throw std::runtime_error("Entry without key already exists");

    if(childPtr.has_value()) {
        entries.insert(entries.begin(), BTreeEntry(std::nullopt, std::nullopt, childPtr));
        this->entryWithoutKeyExists = true;
    }
}

void Node::insertEntry(const BTreeEntry& entry) 
{
    // if(this->entries.size() == 2 * order + 1) <-- user should check if node is full
    //     throw std::runtime_error("Node is full");

    auto position = std::lower_bound(entries.begin(), entries.end(), entry);
    entries.insert(position, entry);

    if (entry.getKey().has_value())
        numberOfKeys++;

    if (numberOfKeys == 2 * order) 
        isFull = true;
}


void Node::setEntries(std::vector<BTreeEntry> entries) 
{
    this->entries = entries;
    countKeys();
}

void Node::setIsLeaf(bool isLeaf) 
{
    this->isLeaf = isLeaf;
}

void Node::setIsFull(bool isFull) 
{
    this->isFull = isFull;
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

void Node::deleteEntryAtIndex(size_t index) 
{
    if(index >= entries.size()) 
        throw std::runtime_error("Index out of range");

    if(entries[index].getKey().has_value())
        numberOfKeys--;

    entries.erase(entries.begin() + index);

    if (numberOfKeys < 2 * order) 
        isFull = false;
}

BTreeEntry Node::popLeftMostEntryWithKey() 
{
    if(entries.empty()) 
        throw std::runtime_error("Node is empty");

    BTreeEntry entry = entries.front();

    if(!entry.getKey().has_value())
    {
        entry = entries[1];
        entries.erase(entries.begin() + 1);
    }
    else
        entries.erase(entries.begin());
        
    numberOfKeys--;

    if (numberOfKeys < 2 * order) 
        isFull = false;

    return entry;
}

BTreeEntry Node::popRightMostEntryWithKey() 
{
    if(entries.empty()) 
        throw std::runtime_error("Node is empty");

    BTreeEntry entry = entries.back();
    entries.pop_back();

    if(entry.getKey().has_value())
        numberOfKeys--;
    else 
        throw std::runtime_error("Entry without key");

    if (numberOfKeys < 2 * order) 
        isFull = false;

    return entry;
}

size_t Node::getMaxNumberOfKeys() const 
{
    return 2 * order;
}

std::optional<BTreeEntry> Node::popEntryWithoutKey() 
{
    if(!entryWithoutKeyExists) 
        return std::nullopt;

    BTreeEntry entry = entries.front();
    if(entry.getKey().has_value()) 
        return std::nullopt;
    
    entries.erase(entries.begin());
    entryWithoutKeyExists = false;

    return entry;
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


uint32_t Node::getNumberOfKeys() const 
{
    return numberOfKeys;
}

BTreeEntry Node::retrieveMedianKeyEntry() const 
{
    return entries[entries.size() / 2];
}

std::pair<std::vector<BTreeEntry>, std::vector<BTreeEntry>> Node::splitNode() 
{
    if(!isFull) 
        throw std::runtime_error("Node is not full");

    size_t middle = entries.size() / 2;

   
    std::vector<BTreeEntry> left(entries.begin(), entries.begin() + middle);
    std::vector<BTreeEntry> right(entries.begin() + middle + 1, entries.end());


    this->clearNode(); // clear the node

    return std::make_pair(left, right);
}


int Node::size(int order) 
{
    int size = 0;

    size += sizeof(uint32_t); // selfPtr
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

    return Node(order, selfPtr, isLeaf, isFull, entries);
}

