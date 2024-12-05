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
    size_t totalSize = sizeof(this->key) + this->record.Size();
    std::unique_ptr<char[]> data = std::make_unique<char[]>(totalSize);
    std::unique_ptr<char[]> recordData = this->record.serialize();
    memcpy(data.get(), recordData.get(), this->record.Size());
    size_t offset = this->record.Size();
    memcpy(data.get() + offset, &this->key, sizeof(this->key));
    return data;
}

std::optional<DataEntry> DataEntry::deserialize(char* data)
{
    std::optional<Record> record = Record::deserialize(data);
    if(!record)
    {
        return std::nullopt;
    }

    uint64_t key;
    size_t offset = Record::Size();
    memcpy(&key , data + offset, sizeof(key));
    
    return DataEntry(*record, key, true);
}

const int32_t DataEntry::Size()
{
    return sizeof(uint64_t) + Record::Size();
}