#include "bTreeIndexManager.hpp"

IndexManager::IndexManager(std::string indexFilePath)
{
    this->indexFilePath = indexFilePath;
    this->IndexFileManager = FileManager(indexFilePath, indexPageSize, indexPageSize);

    if (IndexFileManager.checkIfFileIsEmpty())
    {
        Node root(treeOrder, 0, -1, true);
        IndexFileManager.writeBlockToFile(this->writeBlockIndex, root.serialize().get());
        this->writeBlockIndex++;

    }
    else
    {
        throw std::runtime_error("Index file already exists");
        //do smth later
    }
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
        IndexFileManager.writeBlockToFile(nodeToInsert.getBlockIndex(), nodeToInsert.serialize().get());
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
    auto data = IndexFileManager.readBlockFromFile(blockIndex);
    return Node::deserialize(data.get(), treeOrder).value();
}

