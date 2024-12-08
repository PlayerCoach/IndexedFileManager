#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <cstdint>
#include <memory>


class BTreeEntry
{
    private:
    std::optional<uint64_t> key;
    std::optional<uint32_t> dataBlockPtr;
    std::optional<uint32_t> childPtr; // points to the child node of elements smaller than key

    public:

    BTreeEntry()
    {
        key = std::nullopt;
        dataBlockPtr = std::nullopt;
        childPtr = std::nullopt;
    }

    BTreeEntry(std::optional<uint64_t> key, std::optional<uint32_t> dataBlockPtr, std::optional<uint32_t> childPtr)
        : key(key), dataBlockPtr(dataBlockPtr), childPtr(childPtr) {}

    std::optional<uint64_t> getKey() const { return key; }

    std::optional<uint32_t> getDataBlockPtr() const { return dataBlockPtr; }

    std::optional<uint32_t> getChildPtr() const { return childPtr; }

    void setKey(std::optional<uint64_t> key) { this->key = key; }

    void setDataBlockPtr(std::optional<uint32_t> dataBlockPtr) { this->dataBlockPtr = dataBlockPtr; }

    void setChildPtr(std::optional<uint32_t> childPtr) { this->childPtr = childPtr; }

    bool checkIfEntryIsEmtpy() const{
        return !key.has_value() && !dataBlockPtr.has_value() && !childPtr.has_value();
    }

    static size_t size() {
        size_t size = 0;
        size += sizeof(uint64_t); // key
        size += sizeof(uint32_t); // dataBlockPtr
        size += sizeof(uint32_t); // childPtr
        size += 3*sizeof(bool); // keyExists, dataBlockPtrExists, childPtrExists

        return size;
    }

    std::unique_ptr<char[]> serialize() {
        size_t entrySize = size();
        std::unique_ptr<char[]> data = std::make_unique<char[]>(entrySize);
        char* ptr = data.get();

        // Serialize key
        if (key.has_value()) {
            bool keyExists = true;
            memcpy(ptr, &keyExists, sizeof(bool));
            ptr += sizeof(bool);
            memcpy(ptr, &key.value(), sizeof(uint64_t));
        } else {
            bool keyExists = false;
            memcpy(ptr, &keyExists, sizeof(bool));
            ptr += sizeof(bool);
            uint64_t null = 0;
            memcpy(ptr, &null, sizeof(uint64_t));
        }
        ptr += sizeof(uint64_t);

        // Serialize dataBlockPtr
        if (dataBlockPtr.has_value()) {
            bool dataBlockPtrExists = true;
            memcpy(ptr, &dataBlockPtrExists, sizeof(bool));
            ptr += sizeof(bool);
            memcpy(ptr, &dataBlockPtr.value(), sizeof(uint32_t));
        } else {
            bool dataBlockPtrExists = false;
            memcpy(ptr, &dataBlockPtrExists, sizeof(bool));
            ptr += sizeof(bool);
            uint32_t null = 0;
            memcpy(ptr, &null, sizeof(uint32_t));
        }
        ptr += sizeof(uint32_t);

        // Serialize childPtr
        if (childPtr.has_value()) {
            bool childPtrExists = true;
            memcpy(ptr, &childPtrExists, sizeof(bool));
            ptr += sizeof(bool);
            memcpy(ptr, &childPtr.value(), sizeof(uint32_t));
        } else {
            bool childPtrExists = false;
            memcpy(ptr, &childPtrExists, sizeof(bool));
            ptr += sizeof(bool);
            uint32_t null = 0;
            memcpy(ptr, &null, sizeof(uint32_t));
        }

        return data;
    }
    
static std::optional<BTreeEntry> deserialize(char* data) {
    char* ptr = data;
    std::optional<uint64_t> key = std::nullopt;
    std::optional<uint32_t> dataBlockPtr = std::nullopt; 
    std::optional<uint32_t> childPtr = std::nullopt; 

    // Deserialize key
    bool keyExists;
    memcpy(&keyExists, ptr, sizeof(bool));
    ptr += sizeof(bool);
    if (keyExists) {
        uint64_t keyVal;
        memcpy(&keyVal, ptr, sizeof(uint64_t));
        key = keyVal;
    }
    ptr += sizeof(uint64_t);

    // Deserialize dataBlockPtr

    bool dataBlockPtrExists;
    memcpy(&dataBlockPtrExists, ptr, sizeof(bool));
    ptr += sizeof(bool);

    if (dataBlockPtrExists) {
        uint32_t dataBlockPtrVal;
        memcpy(&dataBlockPtrVal, ptr, sizeof(uint32_t));
        dataBlockPtr = dataBlockPtrVal;
    }

    ptr += sizeof(uint32_t);

    // Deserialize childPtr
    bool childPtrExists;
    memcpy(&childPtrExists, ptr, sizeof(bool));
    ptr += sizeof(bool);

    if (childPtrExists) {
        uint32_t childPtrVal;
        memcpy(&childPtrVal, ptr, sizeof(uint32_t));
        childPtr = childPtrVal;
    }

    return BTreeEntry(key, dataBlockPtr, childPtr);

}

friend bool operator<(const BTreeEntry& a, const BTreeEntry& b) {
    return a.getKey().value() < b.getKey().value();
}

friend bool operator>(const BTreeEntry& a, const BTreeEntry& b) {
    return a.getKey().value() > b.getKey().value();

}

friend bool operator==(const BTreeEntry& a, const BTreeEntry& b) {
    return a.getKey().value() == b.getKey().value();

}

friend bool operator!=(const BTreeEntry& a, const BTreeEntry& b) {
    return a.getKey().value() != b.getKey().value();

}

friend bool operator<=(const BTreeEntry& a, const BTreeEntry& b) {
    return a.getKey().value() <= b.getKey().value();

}

friend bool operator>=(const BTreeEntry& a, const BTreeEntry& b) {
    return a.getKey().value() >= b.getKey().value();

}



};