#include "bTreeIndexManager.hpp"

IndexManager::IndexManager(std::string indexFilePath)
{
    this->indexFilePath = indexFilePath;
    this->IndexFileManager = FileManager(indexFilePath, indexPageSize, indexPageSize);

    if (IndexFileManager.checkIfFileIsEmpty())
    {
        Node root(treeOrder, 0, true);
        IndexFileManager.writeBlockToFile(0, root.serialize().get());
    }
    else
    {
        auto rootData = IndexFileManager.readBlockFromFile(0);
        rootCache = Node::deserialize(rootData.get(), treeOrder).value();
    }
}

void IndexManager::insert(DataEntry dataEntry, uint32_t databaseBlockIndex)
{
    uint64_t key = dataEntry.getKey();
    uint32_t dataBlockPtr = databaseBlockIndex;

    Node nodeToInsert = findLeafNodeForKey(key);
    
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

