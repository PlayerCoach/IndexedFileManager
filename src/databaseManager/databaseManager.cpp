#include "databaseManager.hpp"


DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
{
    this->databaseFilePath = this->databaseFolder + "/" + this->databaseFile;
    this->indexFilePath = this->databaseFolder + "/" + this->indexFile;
    this->databaseFileManager = FileManager(this->databaseFilePath, this->databasePageSize);
    this->createDatabase();
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
    databaseFileManager.openFileForInput();
    std::unique_ptr<char[]> lastBlock = this->databaseFileManager.readLastBlock();
    databaseFileManager.closeFileForInput();

    std::vector<DataEntry> dataEntries = this->deserializeDataBlock(lastBlock.get());
    size_t offset = (dataEntries.size()) * DataEntry::Size();
    std::unique_ptr<char[]> data = dataEntry.serialize();
    memcpy(lastBlock.get() + offset, data.get(), DataEntry::Size());
    databaseFileManager.openFileForOutput();
    this->databaseFileManager.updateLastBlockData(lastBlock.get());
    databaseFileManager.closeFileForOutput();
    if(offset + DataEntry::Size() >= this->databasePageSize)
    {
        databaseFileManager.IncrementIndexOfLastBlock();
    }

}

void DatabaseManager::readDataFromDatabase(const int& index)
{
    databaseFileManager.openFileForInput();
    std::unique_ptr<char[]> blockData = this->databaseFileManager.readBlockFromFile(index);
    databaseFileManager.closeFileForInput();
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
    for (int i = 0; i < this->databasePageSize; i += DataEntry::Size())
    {
        dataEntryOpt = DataEntry::deserialize(data + i);
        if(!dataEntryOpt.has_value())
            break;
        dataEntries.push_back(dataEntryOpt.value());
    }
    return dataEntries;
}
