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

    std::fstream fileStream;

    std::unique_ptr<char[]> lastBlockData;
    size_t lastBlockDataSize;
    
    public:
    FileManager() = default;
    FileManager(std::string fileName, int blockSize);
  
    void openFileStream();
    void closeFileStream();

    const int numberOfBlocksInFile();
    void ensureFileIsOpen();
    bool checkIfFileIsEmpty();


    std::unique_ptr<char[]> readBlockFromFile(int blockIndex);
    std::unique_ptr<char[]> readLastBlock();


    void writeBlockToFile(int blockIndex, char* blockData);
    void updateLastBlockData(char* blockData, size_t actualSize); // data size ??

    void IncrementIndexOfLastBlock();
    


};
