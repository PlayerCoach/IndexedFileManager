#include "fileIOManager.hpp"

FileManager::FileManager()
{
    blockSize = 72;
    indexOfLastBlock = -1;
    fileName = "";
    fileInput = std::ifstream();
    fileOutput = std::ofstream();
}

void FileManager::createFolder(const std::string& folderName)
{
    std::filesystem::create_directory(folderName);
}

std::string FileManager::createFile(const std::string& fileName)
{
    std::ofstream file(fileName);
    file.close();
    return fileName;
}

void FileManager::deleteFolder(const std::string& folderName)
{
    std::filesystem::remove_all(folderName);
}

void FileManager::deleteFile(const std::string& fileName)
{
    std::filesystem::remove(fileName);
}

void FileManager::openFileForInput(const std::string& fileName)
{
    fileInput.open(fileName, std::ios::in | std::ios::binary);
}

void FileManager::openFileForOutput(const std::string& fileName)
{
    fileOutput.open(fileName, std::ios::out | std::ios::binary);
}

void FileManager::closeFileForInput(const std::string& fileName)
{
    fileInput.close();
}

void FileManager::closeFileForOutput(const std::string& fileName)
{
    fileOutput.close();
}


std::unique_ptr<char[]> FileManager::readBlockFromFile(const std::string& fileName, const int& blockIndex, const int& blockSize)
{
    std::unique_ptr<char[]> blockData(new char[blockSize]);
    fileInput.seekg(blockIndex * blockSize);
    fileInput.read(blockData.get(), blockSize);
    return blockData;
}

void FileManager::writeBlockToFile(const std::string& fileName, const int& blockIndex, const int& blockSize, const std::unique_ptr<char[]>& blockData)
{
    fileOutput.seekp(blockIndex * blockSize);
    fileOutput.write(blockData.get(), blockSize);
}

const int FileManager::numberOfBlocksInFile(const std::string& fileName, const int& blockSize)
{
    fileInput.seekg(0, std::ios::end);
    int fileSize = fileInput.tellg();
    if(fileSize == 0)
    {
        return 0;
    }

    return fileSize / blockSize;
}

void FileManager::setBlockSize(const int& blockSize)
{
    this->blockSize = blockSize;
}

std::unique_ptr<char[]> FileManager::readDataFromLastBlock(const std::string& fileName)
{
    if(this->indexOfLastBlock == -1)
    {
        this->indexOfLastBlock = this->numberOfBlocksInFile(fileName, blockSize);
    }
    if(this->indexOfLastBlock != 0)
    {
        fileInput.seekg(this->indexOfLastBlock * this->blockSize);
    }
    std::unique_ptr<char[]> blockData(new char[this->blockSize]);
    fileInput.read(blockData.get(), this->blockSize);
    return blockData;
}

void FileManager::writeDataToLastBlock(const std::string& fileName, const std::unique_ptr<char[]>& blockData, const int& blockSize)
{
    if(this->indexOfLastBlock == -1)
    {
        this->indexOfLastBlock = this->numberOfBlocksInFile(fileName, blockSize);
    }
    if(this->blockSize != blockSize)
    {
        throw std::runtime_error("Block size mismatch");
    }
    if(this->indexOfLastBlock != 0)
    {
        fileOutput.seekp(this->indexOfLastBlock * this->blockSize);
    }
    fileOutput.write(blockData.get(), blockSize);
    this->indexOfLastBlock++;
}



