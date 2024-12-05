#include "dataEntry.hpp"

KeyGen DataEntry::keyGen = KeyGen();

DataEntry::DataEntry()
{
    this->record = Record("random");
    this->key = keyGen.generateKey();

}

DataEntry::DataEntry(const Record& record, uint64_t key, bool skipCheck)
{
    this->record = record;
    this->key = key;

    // Only insert the key if we're not skipping the check
    if (!skipCheck) 
    {
        if (keyGen.keyExists(key)) 
        {
            throw std::invalid_argument("Key already exists");
        }
        keyGen.insertKey(key);
    }
}

// Constructor for user input (with key check)
DataEntry::DataEntry(const Record& record, uint64_t key)
    : DataEntry(record, key, false) // Call the other constructor with skipCheck = false
{
}

const Record& DataEntry::getRecord() const
{
    return this->record;
}

uint64_t DataEntry::getKey() const
{
    return this->key;
}

std::ostream &operator << (std::ostream &os, const DataEntry &dataEntry)
{
    os << "Key: " << dataEntry.key<<" | Record: " << dataEntry.record << std::endl;
    return os;
}

const KeyGen& DataEntry::getKeyGen() const
{
    return keyGen;
}

std::unique_ptr<char[]> DataEntry::serialize() const // total 72 bytes for a data entry 
{
    size_t totalSize = sizeof(this->key) + this->record.getSizeInBytes();
    std::unique_ptr<char[]> data = std::make_unique<char[]>(totalSize);
    memcpy(data.get(), &this->key, sizeof(this->key));
    size_t offset = sizeof(this->key);
    std::unique_ptr<char[]> recordData = this->record.serialize();
    memcpy(data.get() + offset, recordData.get(), this->record.getSizeInBytes());
    return data;
}

std::optional<DataEntry> DataEntry::deserialize(char* data)
{
    uint64_t key;
    memcpy(&key, data, sizeof(key));
    size_t offset = sizeof(key);
    std::optional<Record> record = Record::deserialize(data + offset);
    if(record)
    {
        return DataEntry(record.value(), key, true);
    }
    return std::nullopt;
}

const int32_t DataEntry::getSizeInBytes() const
{
    return sizeof(this->key) + this->record.getSizeInBytes();
}