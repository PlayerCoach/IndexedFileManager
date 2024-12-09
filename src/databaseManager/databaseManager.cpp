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

void DatabaseManager::deleteDatabase(const std::string& databaseName)
{
    std::filesystem::remove_all(databaseName);
}



void DatabaseManager::writeDataToDatabase(DataEntry& dataEntry)
{
    databaseFileManager.openFileStream();
    databaseFileManager.insertDataEntryToLastBlockData(dataEntry.serialize().get());
    databaseFileManager.closeFileStream();
    indexManager->insertPreparation(dataEntry, databaseFileManager.getIndexOfLastBlock()); 
    indexManager->readBTree();

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

std::string DatabaseManager::deleteRecordFromDatabase(const uint64_t& key)
{
    std::string result = this->indexManager->deleteKeyPreparation(key);
    this->indexManager->readBTree();
    return result; 
}


std::vector<DataEntry> DatabaseManager::deserializeDataBlock(char* data)
{
    std::vector<DataEntry> dataEntries;
    std::optional<DataEntry> dataEntryOpt;
    for (int i = 0; i < this->databasePageSize; i += DataEntry::Size())
    {
        dataEntryOpt = DataEntry::deserialize(data + i);
        if(!dataEntryOpt.has_value())
            break;
        dataEntries.push_back(dataEntryOpt.value());
    }
    return dataEntries;
}
