#include "bTreeIndexManager.hpp"

IndexManager::IndexManager(std::string indexFilePath)
{
    this->indexFilePath = indexFilePath;
    this->IndexFileManager = FileManager(indexFilePath, indexPageSize, indexPageSize);

    IndexFileManager.openFileStream();
    if (IndexFileManager.checkIfFileIsEmpty())
    {
        Node root(treeOrder, 0, std::nullopt, true);
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
        throw std::runtime_error("Node is full, not implemented yet");
        this->split(dataEntry, nodeToInsert, key, dataBlockPtr);
        return;
    }

    
    nodeToInsert.insertKey(key, dataBlockPtr);
    if(nodeToInsert.getBlockIndex() == 0)
    {
        rootCache = nodeToInsert;
    }
    IndexFileManager.openFileStream();
    IndexFileManager.writeBlockToFile(nodeToInsert.getBlockIndex(), nodeToInsert.serialize().get());
    IndexFileManager.closeFileStream();
   
    
}

void IndexManager::split(DataEntry& data, Node& node, uint64_t key, uint32_t dataBlockPtr)
{
    if(node.getParentPtr() == -1)
    {
        splitRoot(data, node, key, dataBlockPtr);
    }
  
}

void IndexManager::splitRoot(DataEntry& data, Node& node, uint64_t key, uint32_t dataBlockPtr)
{
    Node leftNode = createNode(node.getIsLeaf(), this->writeBlockIndex);
    this->writeBlockIndex++;
    Node rightNode = createNode(node.getIsLeaf(), this->writeBlockIndex);
    this->writeBlockIndex++;
    BTreeEntry tempEntry(key, dataBlockPtr, std::nullopt);
    std::vector<BTreeEntry> entries = node.getEntries();
    entries.push_back(tempEntry);
    std::sort(entries.begin(), entries.end(), [](const BTreeEntry& a, const BTreeEntry& b) { return a.getKey().value() < b.getKey().value(); });
    size_t middle = entries.size() / 2;
 





    
}

Node IndexManager::findLeafNodeForKey(uint64_t key)
{
    Node currentNode = rootCache;
    while(!currentNode.getIsLeaf())
    {
        for(size_t i = 0; i < currentNode.getNumberOfKeys(); i++)
        {
            if(currentNode.getEntries()[i].getKey().value() > key)
            {
                currentNode = getNode(currentNode.getEntries()[i].getChildPtr().value());
                break;
            }
        }
    }
    return currentNode;
    
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
    if(root.getParentPtr().has_value())
    {
        std::cout << "Root parent ptr: " << root.getParentPtr().value() << std::endl;
    }
    else
    {
        std::cout << "Root has no parent" << std::endl;
    }
    std::cout << "NumberOfKeys: " << root.getNumberOfKeys() << std::endl;
    
   for(size_t i = 0; i < root.getNumberOfKeys(); i++)
   {
       std::cout << "Key: " << root.getEntries()[i].getKey().value() << std::endl;
       std::cout << "DataBlockPtr: " << root.getEntries()[i].getDataBlockPtr().value() << std::endl;
   }
}