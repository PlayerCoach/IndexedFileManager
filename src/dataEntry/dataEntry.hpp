#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "keyGen.hpp"
#include "record.hpp"
#include <optional>
#include <memory>
#include <cstring>


class DataEntry 
{
private:


    Record record;
    uint64_t key;

public:
    static KeyGen keyGen;
    DataEntry();
    DataEntry(const Record& record, uint64_t key);
    DataEntry(const Record& record, uint64_t key, bool skipCheck);
    const Record& getRecord() const;
    uint64_t getKey() const;
    friend std::ostream &operator << (std::ostream &os, const DataEntry &dataEntry);
    const KeyGen& getKeyGen() const;
    std::unique_ptr<char[]> serialize() const;
    static std::optional<DataEntry> deserialize(char* data);
    static const size_t Size();

};

