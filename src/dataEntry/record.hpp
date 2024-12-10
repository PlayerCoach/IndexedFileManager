#pragma once

#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include <vector>
#include <random>
#include <optional>
#include <memory>
#include <cstring>
class Record
{
private:

    static constexpr int32_t MAX_RECORD_COUNT = 15;
    static constexpr int32_t MIN_NUMBER_VALUE = 0;
    static constexpr int32_t MAX_NUMBER_VALUE = 20;
    
    std::set<int32_t> series; 

    void generateRandomSeries();

public:
    Record();
    Record(const std::string& mode);
    Record(std::vector<int32_t> series);
    const std::set<int32_t>& getSeries() const;
    Record& operator = (const Record &record);
    void insert(int32_t value);
    void remove(int32_t value);
    const int32_t getSize() const;
    static const size_t Size();
    friend std::ostream &operator << (std::ostream &os, const Record &record);

    static int32_t getMaxRecordCount();
    static int32_t getMinNumberValue();
    static int32_t getMaxNumberValue();

    std::unique_ptr<char[]> serialize() const;
    static std::optional<Record> deserialize(char* data);
};
