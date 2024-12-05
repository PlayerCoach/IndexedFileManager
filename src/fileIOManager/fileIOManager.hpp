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
    bool isFileInputOpen = false;

    bool isFileOutputOpen = false;
    std::ofstream fileOutput;
    
    public:
    FileManager() = default;
    FileManager(std::string fileName, int blockSize);
  
    const int numberOfBlocksInFile();

    void openFileForInput(); // add error handling if file is open for output
    void openFileForOutput(); // add error handling if file is open for input

    void closeFileForInput();
    void closeFileForOutput();

    std::unique_ptr<char[]> readBlockFromFile(int blockIndex);
    void writeBlockToFile(int blockIndex, char* blockData);


    std::unique_ptr<char[]> readLastBlock();
    void updateLastBlockData(char* blockData, size_t actualSize); // data size ??

    void IncrementIndexOfLastBlock();
    


};
