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
    int dataSize;
    int indexOfLastBlock = -1;
    std::string fileName;

    std::fstream fileStream;

    std::unique_ptr<char[]> lastBlockData;
    size_t lastBlockDataSize = 0;
    bool lastBlockDataDirty = false; // conent of last block is in buffer and possibly not written to file
    
    public:
    FileManager() = default;
    FileManager(std::string fileName, int blockSize, int dataSize);
  
    void openFileStream();
    void closeFileStream();

    const int numberOfBlocksInFile();
    void ensureFileIsOpen();
    bool checkIfFileIsEmpty();


    std::unique_ptr<char[]> readBlockFromFile(int blockIndex);
    std::unique_ptr<char[]> readLastBlock();


    void writeBlockToFile(int blockIndex, char* blockData);
    void updateLastBlockData(char* blockData, size_t actualSize); // data size ??

    void insertDataEntryToLastBlockData(char* data);
    void flushLastBlockData();

    void IncrementIndexOfLastBlock();
    uint32_t getIndexOfLastBlock() { return indexOfLastBlock; }
    void setLastBlockData(char* newLastBlockData, size_t size);




    
    


};
