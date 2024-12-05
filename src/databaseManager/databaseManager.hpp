#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include "fileIOManager.hpp"
#include "dataEntry.hpp"
#include <filesystem>
#include <optional>

class DatabaseManager
{
    public:
    static DatabaseManager& getInstance();

    private:
    FileManager databaseFileManager;
    const std::string databaseFolder = "database";
    const std::string databaseFile = "database.bin";
    const std::string indexFile = "index.bin";

    std::string databaseFilePath;
    std::string indexFilePath;

    const int databasePageSize = 72;

    DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    public:
    void deleteDatabase(const std::string& databaseName);

    void writeDataToDatabase(DataEntry& dataEntry);
    void readDataBlockFromDatabase(const int& blockIndex);
    void readAllDataFromDatabase();

};