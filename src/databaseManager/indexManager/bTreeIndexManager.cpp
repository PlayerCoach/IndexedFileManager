#include "bTreeIndexManager.hpp"

IndexManager::IndexManager(std::string indexFilePath)
{
    this->indexFilePath = indexFilePath;
    this->IndexFileManager = FileManager(indexFilePath, indexPageSize, indexPageSize);

    IndexFileManager.openFileStream();
    if (IndexFileManager.checkIfFileIsEmpty())
    {
        Node root(treeOrder, 0, -1, true);
        IndexFileManager.writeBlockToFile(this->writeBlockIndex, root.serialize().get());
        this->writeBlockIndex++;
        rootCache = root;

        

    }
    else
    {
        IndexFileManager.closeFileStream();
        throw std::runtime_error("Index file already exists");
        //do smth later
    }
    IndexFileManager.closeFileStream();
}

void IndexManager::insert(DataEntry dataEntry, uint32_t databaseBlockIndex)
{
    uint64_t key = dataEntry.getKey();
    uint32_t dataBlockPtr = databaseBlockIndex;

    Node nodeToInsert = findLeafNodeForKey(key);
  
    if(nodeToInsert.getIsFull())
    {
        //split node
    }
    auto position = getInsertPosition(nodeToInsert, key);
    if(position.has_value())
    {
        nodeToInsert.insertKey(key, dataBlockPtr, position.value());
        if(nodeToInsert.getBlockIndex() == 0)
        {
            rootCache = nodeToInsert;
        }
        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(nodeToInsert.getBlockIndex(), nodeToInsert.serialize().get());
        IndexFileManager.closeFileStream();
    }
    else
    {
        //split node
    }
    
}

Node IndexManager::findLeafNodeForKey(uint64_t key)
{
    Node currentNode = rootCache;
    while (!currentNode.getIsLeaf())
    {
        size_t position = 0;
        for (const auto& pair : currentNode.getKeyDataPairs())
        {
            if (key > pair.first)
            {
                position++;
            }
            else
            {
                break;
            }
        }
        currentNode = getNode(currentNode.getChildPtrs()[position]);
    }
    return currentNode;
}

//maby move it to node class
std::optional<size_t> IndexManager::getInsertPosition(Node& node, uint64_t key)
{
    if(node.getIsFull())
    {
        return std::nullopt;
    }

    size_t position = 0;
    for (const auto& pair : node.getKeyDataPairs())
    {
        if (key > pair.first)
        {
            position++;
        }
        else
        {
            break;
        }
    }
    return position;
}

Node IndexManager::getNode(uint32_t blockIndex)
{
    IndexFileManager.openFileStream();
    auto data = IndexFileManager.readBlockFromFile(blockIndex);
    IndexFileManager.closeFileStream();
    return Node::deserialize(data.get(), treeOrder).value();
}

Node IndexManager::createNode(bool isLeaf, uint32_t blockIndex)
{
    return Node(treeOrder, blockIndex, -1, isLeaf);
}

void IndexManager::readBTree()
{
    Node root = getNode(0);
    std::cout << "Root: " << root.getBlockIndex() << std::endl;
    std::cout << "Root is leaf: " << root.getIsLeaf() << std::endl;
    std::cout << "Root is full: " << root.getIsFull() << std::endl;
    std::cout << "ParentPtr: " << root.getParentPtr() << std::endl;
    std::cout << "NumberOfKeys: " << root.getNumberOfKeys() << std::endl;
    
    for (const auto& pair : root.getKeyDataPairs())
    {
        std::cout << "Key: " << pair.first << " DataBlockPtr: " << pair.second << std::endl;
    }

    for (const auto& childPtr : root.getChildPtrs())
    {
        Node child = getNode(childPtr);
        std::cout << "Child: " << child.getBlockIndex() << std::endl;
        for (const auto& pair : child.getKeyDataPairs())
        {
            std::cout << "Key: " << pair.first << " DataBlockPtr: " << pair.second << std::endl;
        }
    }
}