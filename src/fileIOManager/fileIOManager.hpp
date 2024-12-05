#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <filesystem>


class FileManager
{   
    private:
    int blockSize;
    int indexOfLastBlock;
    std::string fileName;
    std::ifstream fileInput;
    std::ofstream fileOutput;
    
    public:
    FileManager();
    void createFolder(const std::string& folderName);
    std::string createFile(const std::string& fileName);
    void deleteFolder(const std::string& folderName);
    void deleteFile(const std::string& fileName);

    void openFileForInput(const std::string& fileName);
    void openFileForOutput(const std::string& fileName);

    void closeFileForInput(const std::string& fileName);
    void closeFileForOutput(const std::string& fileName);

    std::unique_ptr<char[]> readBlockFromFile(const std::string& fileName, const int& blockIndex, const int& blockSize);
    void writeBlockToFile(const std::string& fileName, const int& blockIndex, const int& blockSize, const std::unique_ptr<char[]>& blockData);\

    const int numberOfBlocksInFile(const std::string& fileName, const int& blockSize);

    void setBlockSize(const int& blockSize);
    std::unique_ptr<char[]> readDataFromLastBlock(const std::string& fileName);
    void writeDataToLastBlock(const std::string& fileName, const std::unique_ptr<char[]>& blockData, const int& blockSize);


};
