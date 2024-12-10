#include "fileIOManager.hpp"

FileManager::FileManager(std::string fileName, int blockSize, int dataSize)
    : fileName(std::move(fileName)), blockSize(blockSize), dataSize(dataSize)
{
    lastBlockData = std::unique_ptr<char[]>(new char[blockSize]);
    fileStream = std::fstream();
}

void FileManager::openFileStream()
{
    fileStream.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
    if (!fileStream)
        throw std::runtime_error("Failed to open the file");

    if (indexOfLastBlock == -1)
        indexOfLastBlock = numberOfBlocksInFile();
}

void FileManager::closeFileStream()
{
    fileStream.close();
}

const int FileManager::numberOfBlocksInFile()
{
    fileStream.seekg(0, std::ios::end);
    int fileSize = static_cast<int>(fileStream.tellg());
    if(fileSize == 0)
    {
        return 0;
    }

    return fileSize / blockSize;
}

void FileManager::ensureFileIsOpen()
{
    if (!fileStream.is_open())
        throw std::runtime_error("File is not open");
}

bool FileManager::checkIfFileIsEmpty()
{
    this->ensureFileIsOpen();
    return fileStream.peek() == std::ifstream::traits_type::eof();
}

std::unique_ptr<char[]> FileManager::readBlockFromFile(int blockIndex)
{
    this->ensureFileIsOpen();
    std::unique_ptr<char[]> blockData(new char[blockSize]);
    memset(blockData.get(), 0, blockSize);
    fileStream.seekg(blockIndex * blockSize);
    fileStream.read(blockData.get(), blockSize);
    return blockData;
}

std::unique_ptr<char[]> FileManager::readLastBlock()
{
    ensureFileIsOpen();

    if (indexOfLastBlock == 0 && checkIfFileIsEmpty())
    {
        std::unique_ptr<char[]> emptyBlock(new char[blockSize]);
        memset(emptyBlock.get(), 0, blockSize); // check if -1 doesn't cause any problems
        return emptyBlock;
    }
        
   return readBlockFromFile(indexOfLastBlock);
}

void FileManager::writeBlockToFile(int blockIndex, char* blockData)
{
    std::unique_ptr<char[]> blockDataCopy(new char[blockSize]);
    memset(blockDataCopy.get(), 0, blockSize);
    memcpy(blockDataCopy.get(), blockData, blockSize);
    fileStream.seekp(blockIndex * blockSize);
    fileStream.write(blockData, blockSize);
}


void FileManager::updateLastBlockData(char* blockData, size_t actualSize)
{
    // is this equivalent to flush???
    ensureFileIsOpen();
    if (indexOfLastBlock != 0)
        fileStream.seekp(indexOfLastBlock * blockSize);
    fileStream.write(blockData, actualSize);
}

void FileManager::insertDataEntryToLastBlockData(char* data)
{
    if(this->blockSize%dataSize != 0)
    {
        // possibly trying to write data of wrong size
        throw std::runtime_error("Data size is not a multiple of block size"); // 
    }

    if (lastBlockDataSize + dataSize > blockSize)
    {
       flushLastBlockData();
    }
       
    memcpy(lastBlockData.get() + lastBlockDataSize, data, dataSize);
    lastBlockDataSize += dataSize;
    this->lastBlockDataDirty = true;

}

void FileManager::flushLastBlockData()
{
    /*write last block data to file */
    this->updateLastBlockData(lastBlockData.get(), lastBlockDataSize);

    /* if last block data was full then increment index of last block and set last block data size to 0 */
    if(lastBlockDataSize >= blockSize)
    {
        memset(lastBlockData.get(), 0, blockSize);
        lastBlockDataSize = 0;
        this->IncrementIndexOfLastBlock();
        this->lastBlockDataDirty = false;
    }
    /* if last block data was not full then keep it still in RAM for future operations*/
    else
    {
        this->lastBlockDataDirty = true;
    }

}

void FileManager::setLastBlockData(char* newLastBlockData, size_t size)
{
    memset(lastBlockData.get(), 0, blockSize);
    memcpy(lastBlockData.get(), newLastBlockData, size);
    lastBlockDataSize = size;
    this->lastBlockDataDirty = true;
}

void FileManager::IncrementIndexOfLastBlock()
{
    this->indexOfLastBlock++;

}




