#include "fileIOManager.hpp"

FileManager::FileManager(std::string fileName, int blockSize)
{
    this->fileName = fileName;
    this->blockSize = blockSize;
    this->indexOfLastBlock = -1;
    this->fileInput = std::ifstream();
    this->fileOutput = std::ofstream();
}

const int FileManager::numberOfBlocksInFile()
{
    fileInput.seekg(0, std::ios::end);
    int fileSize = static_cast<int>(fileInput.tellg());
    if(fileSize == 0)
    {
        return 0;
    }

    return fileSize / blockSize;
}

void FileManager::openFileForInput()
{
    if(this->isFileOutputOpen)
        throw std::runtime_error("File is open for output");

    fileInput.open(fileName, std::ios::in | std::ios::binary);
    this->isFileInputOpen = true;

    if(this->indexOfLastBlock == -1)
        this->indexOfLastBlock = this->numberOfBlocksInFile();
}

void FileManager::openFileForOutput()
{
    if(this->isFileInputOpen)
        throw std::runtime_error("File is open for input");

    fileOutput.open(fileName, std::ios::out | std::ios::binary);
    this->isFileOutputOpen = true;

    if(this->indexOfLastBlock == -1)
        this->indexOfLastBlock = this->numberOfBlocksInFile();
}

void FileManager::closeFileForInput()
{
    fileInput.close();
    this->isFileInputOpen = false;
}

void FileManager::closeFileForOutput()
{
    fileOutput.close();
    this->isFileOutputOpen = false;
}


std::unique_ptr<char[]> FileManager::readBlockFromFile(int blockIndex)
{
    if(!this->isFileInputOpen)
        throw std::runtime_error("File is not open for input");

    std::unique_ptr<char[]> blockData(new char[blockSize]);
    fileInput.seekg(blockIndex * blockSize);
    fileInput.read(blockData.get(), blockSize);
    return blockData;
}

void FileManager::writeBlockToFile(int blockIndex, char* blockData)
{
    if(!this->isFileOutputOpen)
        throw std::runtime_error("File is not open for output");

    fileOutput.seekp(blockIndex * blockSize);
    fileOutput.write(blockData, blockSize);
}


std::unique_ptr<char[]> FileManager::readLastBlock()
{
    if (!this->isFileInputOpen)
        throw std::runtime_error("File is not open for input");

    std::unique_ptr<char[]> blockData(new char[this->blockSize]);
    memset(blockData.get(), 0, this->blockSize); // Initialize with zeros

    if (this->indexOfLastBlock > 0)
    {
        fileInput.seekg((this->indexOfLastBlock) * this->blockSize);
    }
    else
    {
        // Handle the case where the file is empty
        fileInput.seekg(0, std::ios::end);
        if (fileInput.tellg() == 0)
        {
            return blockData;  // Return an empty block
        }
    }

    fileInput.read(blockData.get(), this->blockSize);
    return blockData;
}

void FileManager::updateLastBlockData(char *blockData)
{
    if(!this->isFileOutputOpen)
        throw std::runtime_error("File is not open for output");

    if(this->indexOfLastBlock != 0)
    {
        fileOutput.seekp(this->indexOfLastBlock * this->blockSize);
    }
    fileOutput.write(blockData, this->blockSize);
    //this->indexOfLastBlock++;
}




