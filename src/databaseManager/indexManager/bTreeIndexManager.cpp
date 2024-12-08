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
    if(!node.getParentPtr().has_value())
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
    BTreeEntry ascendedNode = entries[middle];
    leftNode.setEntries(std::vector<BTreeEntry>(entries.begin(), entries.begin() + middle));
    rightNode.setEntries(std::vector<BTreeEntry>(entries.begin() + middle + 1, entries.end()));
    //insert the ptr of ascended node to the most left  of the right node
    rightNode.insertChildPtr(ascendedNode.getDataBlockPtr().value());
    ascendedNode.setChildPtr(rightNode.getBlockIndex());

    leftNode.setParentPtr(0);
    rightNode.setParentPtr(0);
    leftNode.setIsFull(false);
    rightNode.setIsFull(false);
    leftNode.setNumberOfKeys(leftNode.getEntries().size());
    rightNode.setNumberOfKeys(rightNode.getEntries().size());
    leftNode.setIsLeaf(node.getIsLeaf());
    rightNode.setIsLeaf(node.getIsLeaf());
    IndexFileManager.openFileStream();
    IndexFileManager.writeBlockToFile(leftNode.getBlockIndex(), leftNode.serialize().get());
    IndexFileManager.writeBlockToFile(rightNode.getBlockIndex(), rightNode.serialize().get());
    IndexFileManager.closeFileStream();

    IndexFileManager.openFileStream();
    std::unique_ptr<char[]> readNode = IndexFileManager.readBlockFromFile(1);
    IndexFileManager.closeFileStream();
    Node deserializedNode = Node::deserialize(readNode.get(), treeOrder).value();


    node.clearNode();
    node.insertChildPtr(leftNode.getBlockIndex()); // this is 2 for some reason should be 1
    node.insertEntry(ascendedNode);
    node.setIsLeaf(false);
    node.setIsFull(false);
    node.setNumberOfKeys(1);
    IndexFileManager.openFileStream();
    IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
    IndexFileManager.closeFileStream();
    rootCache = node;
    return;
    
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
    std::queue<Node> q;
    q.push(rootCache);
    while(!q.empty())
    {
        Node currentNode = q.front();
        q.pop();
        readNode(currentNode);
        for(auto entry : currentNode.getEntries())
        {
            if(entry.getChildPtr().has_value())
            {
                q.push(getNode(entry.getChildPtr().value()));
            }
        }
    }
}

void IndexManager::readNode(Node& node)
{
    std::cout << "Node: " << node.getBlockIndex() << std::endl;
    std::cout << "Node is leaf: " << node.getIsLeaf() << std::endl;
    std::cout << "Node is full: " << node.getIsFull() << std::endl;
    if(node.getParentPtr().has_value())
    {
        std::cout << "Node parent ptr: " << node.getParentPtr().value() << std::endl;
    }
    else
    {
        std::cout << "Node has no parent" << std::endl;
    }

    std::cout << "NumberOfKeys: " << node.getNumberOfKeys() << std::endl;
    
   for(auto entry : node.getEntries())
   {
    if(entry.getKey().has_value())
    {
        std::cout << "Key: " << entry.getKey().value() << " DataBlockPtr: " << entry.getDataBlockPtr().value() << std::endl;
    }
    if(entry.getChildPtr().has_value())
    {
        std::cout << "ChildPtr: " << entry.getChildPtr().value() << std::endl;
    }
       
   }

}