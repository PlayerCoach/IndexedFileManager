#include "databaseManager.hpp"


DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
{
    databaseFileManager.createFolder(databaseFolder);
    this->databaseFilePath = databaseFileManager.createFile(databaseFolder + "/" + databaseFile);
    this->indexFilePath = databaseFileManager.createFile(databaseFolder + "/" + indexFile);

}

void DatabaseManager::writeDataToDatabase(DataEntry& dataEntry)
{
    std::unique_ptr<char[]> dataEntryBuffer = dataEntry.serialize();
    databaseFileManager.openFileForInput(databaseFilePath);
    std::unique_ptr<char[]> lastBlock = databaseFileManager.readDataFromLastBlock(databaseFilePath);
    databaseFileManager.closeFileForInput(databaseFilePath);

    //later here will i will be appending data to the last block, but lets ignore it for now cause block size is 72

    databaseFileManager.openFileForOutput(databaseFilePath);
    databaseFileManager.writeDataToLastBlock(databaseFilePath, dataEntryBuffer, databasePageSize);
    databaseFileManager.closeFileForOutput(databaseFilePath);

    


}


