#include "dataEntry.hpp"

KeyGen DataEntry::keyGen = KeyGen();

DataEntry::DataEntry()
{
    this->record = Record("random");
    this->size = this->record.getSize();
    this->key = keyGen.generateKey();

}

DataEntry::DataEntry(const Record& record, uint64_t key)
{
    try
    {
        if(keyGen.keyExists(key))
        {
            throw std::invalid_argument("Key already exists");
        }
        keyGen.insertKey(key);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        
    }
    
    this->record = record;
    this->size = this->record.getSize();
   
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
