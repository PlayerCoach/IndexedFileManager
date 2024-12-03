#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "keyGen.hpp"
#include "record.hpp"
#include <optional>


class DataEntry 
{
private:

    static KeyGen keyGen;

    int32_t size;
    Record record;
    uint64_t key;

public:
    DataEntry();
    DataEntry(const Record& record, uint64_t key);
    const Record& getRecord() const;
    uint64_t getKey() const;

    const KeyGen& getKeyGen() const;

};

