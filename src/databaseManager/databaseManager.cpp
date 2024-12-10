#include "databaseManager.hpp"


DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
{
    this->lastBlockBuffer = std::make_unique<char[]>(this->databasePageSize);
    this->databaseFilePath = this->databaseFolder + "/" + this->databaseFile;
    this->indexFilePath = this->databaseFolder + "/" + this->indexFile;
    this->databaseFileManager = FileManager(this->databaseFilePath, this->databasePageSize, DataEntry::Size());
    this->createDatabase();
    this->indexManager = std::make_unique<IndexManager>(this->indexFilePath);

}

void DatabaseManager::createDatabase()
{
    if (!std::filesystem::exists(this->databaseFolder))
        std::filesystem::create_directory(this->databaseFolder);

    if (!std::filesystem::exists(this->databaseFilePath))
    {
        std::ofstream databaseFile(this->databaseFilePath, std::ios::binary);
        databaseFile.close();
    }

    if (!std::filesystem::exists(this->indexFilePath))
    {
        std::ofstream indexFile(this->indexFilePath, std::ios::binary);
        indexFile.close();
    }
}

void DatabaseManager::deleteDatabase()
{
    std::filesystem::remove_all(this->databaseFolder);
}



std::string DatabaseManager::writeDataToDatabase(DataEntry& dataEntry)
{
    databaseFileManager.openFileStream();
    databaseFileManager.insertDataEntryToLastBlockData(dataEntry.serialize().get());
    databaseFileManager.closeFileStream();
    std::string status;
    status = indexManager->insertPreparation(dataEntry, databaseFileManager.getIndexOfLastBlock()); 

    if(status != "")
    {
        return status;
    }
    return "Data written to database";

}

std::string DatabaseManager::deleteRecordFromDatabase(const uint64_t& key)
{
    this->databaseFileManager.openFileStream();
    this->databaseFileManager.flushLastBlockData();// flush last block data to file
    this->databaseFileManager.closeFileStream();
    std::optional<uint32_t> result = this->indexManager->deleteKeyPreparation(key);
    if(!result.has_value())
    {
        return "Key not found";
    }

    this->databaseFileManager.openFileStream();
    std::unique_ptr<char[]> blockData = this->databaseFileManager.readBlockFromFile(result.value());
    this->databaseFileManager.closeFileStream();

    std::vector<DataEntry> dataEntries = this->deserializeDataBlock(blockData.get());
    auto it = std::find_if(dataEntries.begin(), dataEntries.end(), [key](DataEntry& dataEntry) { return dataEntry.getKey() == key; });
    if (it != dataEntries.end())
    {
        dataEntries.erase(it);
    }

    this->databaseFileManager.openFileStream();
    this->databaseFileManager.writeBlockToFile(result.value(), this->serializeDataBlock(dataEntries).get());
    if(result.value() == 0)
    {
        this->databaseFileManager.setLastBlockData(this->serializeDataBlock(dataEntries).get(), dataEntries.size() * DataEntry::Size());
    }
    this->databaseFileManager.closeFileStream();
  
    return "Record deleted";
}

std::string DatabaseManager::searchForDataInDatabase(const uint64_t& key)
{
    this->databaseFileManager.openFileStream();
    this->databaseFileManager.flushLastBlockData();
    this->databaseFileManager.closeFileStream();
    
    std::optional<uint32_t> blockPtr = this->indexManager->search(key);
    if(!blockPtr.has_value())
    {
        return "Key not found";
    }
    this->databaseFileManager.openFileStream();
    std::unique_ptr<char[]> blockData = this->databaseFileManager.readBlockFromFile(blockPtr.value());
    this->databaseFileManager.closeFileStream();
    std::vector<DataEntry> dataEntries = this->deserializeDataBlock(blockData.get());
    auto it = std::find_if(dataEntries.begin(), dataEntries.end(), [key](DataEntry& dataEntry) { return dataEntry.getKey() == key; });
    if (it != dataEntries.end())
    {
        std::cout << *it << std::endl;
    }
    return "";
}

std::string DatabaseManager::updateRecordInDatabase(const uint64_t& key, const Record& record)
{
    this->databaseFileManager.openFileStream();
    this->databaseFileManager.flushLastBlockData();
    this->databaseFileManager.closeFileStream();
    std::optional<uint32_t> blockPtr = this->indexManager->search(key);
    if(!blockPtr.has_value())
    {
        return "Key not found";
    }
    this->databaseFileManager.openFileStream();
    std::unique_ptr<char[]> blockData = this->databaseFileManager.readBlockFromFile(blockPtr.value());
    this->databaseFileManager.closeFileStream();
    std::vector<DataEntry> dataEntries = this->deserializeDataBlock(blockData.get());
    auto it = std::find_if(dataEntries.begin(), dataEntries.end(), [key](DataEntry& dataEntry) { return dataEntry.getKey() == key; });
    if (it != dataEntries.end())
    {
        DataEntry updatedDataEntry = DataEntry(record, key);
        *it = updatedDataEntry;
    }
    this->databaseFileManager.openFileStream();
    this->databaseFileManager.writeBlockToFile(blockPtr.value(), this->serializeDataBlock(dataEntries).get());
    if(blockPtr.value() == 0)
    {
        this->databaseFileManager.setLastBlockData(this->serializeDataBlock(dataEntries).get(), dataEntries.size() * DataEntry::Size());
    }
    this->databaseFileManager.closeFileStream();
    return "Record updated";
}

void DatabaseManager::readDataFromDatabase(const int& index)
{
    databaseFileManager.openFileStream();
    std::unique_ptr<char[]> blockData = this->databaseFileManager.readBlockFromFile(index);
    databaseFileManager.closeFileStream();
    std::vector<DataEntry> dataEntries = this->deserializeDataBlock(blockData.get());
    if(dataEntries.empty())
    {
        std::cout << "No data in this block" << std::endl;
        return;
    }
    for (auto& dataEntry : dataEntries)
    {
        std::cout << dataEntry << std::endl;
    }
}


std::vector<DataEntry> DatabaseManager::deserializeDataBlock(char* data)
{
    std::vector<DataEntry> dataEntries;
    std::optional<DataEntry> dataEntryOpt;
    for (size_t i = 0; i < this->databasePageSize; i += DataEntry::Size())
    {
        dataEntryOpt = DataEntry::deserialize(data + i);
        if(!dataEntryOpt.has_value())
            continue; // break can be here if i will handle free spaces
        dataEntries.push_back(dataEntryOpt.value());
    }
    return dataEntries;
}

std::unique_ptr<char[]> DatabaseManager::serializeDataBlock(std::vector<DataEntry>& dataEntries)
{
    std::unique_ptr<char[]> data = std::make_unique<char[]>(this->databasePageSize);
    size_t offset = 0;
    for (auto& dataEntry : dataEntries)
    {
        std::unique_ptr<char[]> serializedDataEntry = dataEntry.serialize();
        memcpy(data.get() + offset, serializedDataEntry.get(), DataEntry::Size());
        offset += DataEntry::Size();
    }
    return data;
}

void DatabaseManager::readAllDataFromDatabase()
{
    this->databaseFileManager.openFileStream();
    this->databaseFileManager.flushLastBlockData();
    this->databaseFileManager.closeFileStream();

    int numberOfBlocks = databaseFileManager.getIndexOfLastBlock();
    for (int i = 0; i <= numberOfBlocks; i++)
    {
        readDataFromDatabase(i);
    }
}

void DatabaseManager::readBTree()
{
    this->indexManager->readBTree();
}

void DatabaseManager::getFilesProportions()
{
    int numberOfRecords = this->indexManager->getNumberOfKeysInTree();
    int totalSizeofrecords = numberOfRecords * DataEntry::Size();
    int numberOfPages = this->indexManager->getNumberOfPages();
    int totalSizeOfDatabase = numberOfPages * this->databasePageSize;

    std::cout << "Space Used by Database: " << totalSizeOfDatabase << " bytes" << std::endl;
}
