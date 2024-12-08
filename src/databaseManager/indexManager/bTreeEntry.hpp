#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <cstdint>
#include <memory>
#include <cstring>
#include "serializeOptional.hpp"


class BTreeEntry
{

private:
    std::optional<uint64_t> key;
    std::optional<uint32_t> dataBlockPtr;
    std::optional<uint32_t> childPtr; // points to the child node larger than the key

public:

    BTreeEntry() 
        : key(std::nullopt), dataBlockPtr(std::nullopt), childPtr(std::nullopt) {} 

    BTreeEntry(std::optional<uint64_t> key, std::optional<uint32_t> dataBlockPtr, std::optional<uint32_t> childPtr)
        : key(key), dataBlockPtr(dataBlockPtr), childPtr(childPtr) {}


    std::optional<uint64_t> getKey() const { return key; }
    std::optional<uint32_t> getDataBlockPtr() const { return dataBlockPtr; }
    std::optional<uint32_t> getChildPtr() const { return childPtr; }


    void setKey(std::optional<uint64_t> key) { this->key = key; }
    void setDataBlockPtr(std::optional<uint32_t> dataBlockPtr) { this->dataBlockPtr = dataBlockPtr; }
    void setChildPtr(std::optional<uint32_t> childPtr) { this->childPtr = childPtr; }


    bool checkIfEntryIsEmtpy() const
    {
        return !key.has_value() && !dataBlockPtr.has_value() && !childPtr.has_value();
    }

    static size_t size() 
    {
        size_t size = 0;
        
        size += sizeOptional<uint64_t>();
        size += sizeOptional<uint32_t>();
        size += sizeOptional<uint32_t>();

        return size;
    }

    std::unique_ptr<char[]> serialize() const 
    {
        size_t entrySize = size();
        std::unique_ptr<char[]> data = std::make_unique<char[]>(entrySize);
        char* ptr = data.get();

        serializeOptional(key, ptr);
        serializeOptional(dataBlockPtr, ptr);
        serializeOptional(childPtr, ptr);

        return data;
    }
    
    static std::optional<BTreeEntry> deserialize(char* data) 
    {
        char* ptr = data;

        std::optional<uint64_t> key = deserializeOptional<uint64_t>(ptr);
        std::optional<uint32_t> dataBlockPtr = deserializeOptional<uint32_t>(ptr);
        std::optional<uint32_t> childPtr = deserializeOptional<uint32_t>(ptr);

        return BTreeEntry(key, dataBlockPtr, childPtr);

    }

    friend bool operator<(const BTreeEntry& a, const BTreeEntry& b) 
    {
        if(!a.getKey().has_value() && !b.getKey().has_value())
            return false;

        if(!a.getKey().has_value())
            return true;

        if(!b.getKey().has_value())
            return false;

        return a.getKey().value() < b.getKey().value();
    }

    friend bool operator>(const BTreeEntry& a, const BTreeEntry& b) {

        if(!a.getKey().has_value() && !b.getKey().has_value())
            return false;

        if(!a.getKey().has_value())
            return false;

        if(!b.getKey().has_value())
            return true;

        return a.getKey().value() > b.getKey().value();

    }

    friend bool operator==(const BTreeEntry& a, const BTreeEntry& b) 
    {
        return a.getKey().value() == b.getKey().value();
    }

    friend bool operator!=(const BTreeEntry& a, const BTreeEntry& b) 
    {
        return a.getKey().value() != b.getKey().value();

    }

    friend bool operator<=(const BTreeEntry& a, const BTreeEntry& b) 
    {
        return a == b || a < b;

    }

    friend bool operator>=(const BTreeEntry& a, const BTreeEntry& b) 
    {
        return a == b || a > b;

    }

};