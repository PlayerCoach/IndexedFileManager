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

void IndexManager::insertPreparation(DataEntry DataEntry, uint32_t databaseBlockIndex)
{
    BTreeEntry entry(DataEntry.getKey(), databaseBlockIndex, std::nullopt);
    this->insertToLeaf(entry);

}

    
void IndexManager::insertToLeaf(BTreeEntry entry)
{
    if(entry.getKey().has_value() == false)
    {
        throw std::runtime_error("Key is not set");
    }


    Node nodeToInsert = findLeafNodeForKey(entry.getKey().value());
    if(nodeToInsert.getIsFull())
    {
        if(!checkIfCanCompensate(nodeToInsert, entry))
            this->split(nodeToInsert, entry);
        
        return ; // if we split, we dont want to insert again
    }

    
    nodeToInsert.insertEntry(entry);
    if(nodeToInsert.getBlockIndex() == 0)
    {
        rootCache = nodeToInsert;
    }
    IndexFileManager.openFileStream();
    IndexFileManager.writeBlockToFile(nodeToInsert.getBlockIndex(), nodeToInsert.serialize().get());
    IndexFileManager.closeFileStream();
   
    
}

void IndexManager::insertToNode(Node& node, BTreeEntry entry)
{
   
    if(node.getIsFull())
    {
        if(!checkIfCanCompensate(node, entry))
            this->split(node, entry);
        return;
    }

    node.insertEntry(entry);
    if(node.getBlockIndex() == 0)
    {
        rootCache = node;
    }
    IndexFileManager.openFileStream();
    IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
    IndexFileManager.closeFileStream();
    
}

void IndexManager::split(Node& node, BTreeEntry entry)
{
    if(!node.getParentPtr().has_value())
    {
        splitRoot(node, entry);
        return;
    }

    Node parentNode = getNode(node.getParentPtr().value());
    Node rightNode = createNode(node.getIsLeaf(), this->writeBlockIndex);
    this->writeBlockIndex++;
    // current node will be left node
    node.insertEntry(entry);
    BTreeEntry ascendedEntry = node.retrieveMedianKeyEntry();
    std::pair<std::vector<BTreeEntry>, std::vector<BTreeEntry>> splitEntries = node.splitNode(); // split cleans the node
    node.setParentPtr(parentNode.getBlockIndex()); // <- issue somewhere here, parent ptr is not set correctly

    rightNode.setEntries(splitEntries.second);
    rightNode.insertChildPtr(ascendedEntry.getChildPtr());
    rightNode.setParentPtr(parentNode.getBlockIndex());
    rightNode.setIsFull(false);

    //insert ptr to right node to the ascended entry
    ascendedEntry.setChildPtr(rightNode.getBlockIndex());

    node.setEntries(splitEntries.first);
    node.setIsFull(false);
    IndexFileManager.openFileStream();
    IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
    IndexFileManager.writeBlockToFile(rightNode.getBlockIndex(), rightNode.serialize().get());
    IndexFileManager.closeFileStream();

    this->insertToNode(parentNode, ascendedEntry);

}

void IndexManager::splitRoot(Node& node, BTreeEntry entry)
{
    Node leftNode = createNode(node.getIsLeaf(), this->writeBlockIndex);
    this->writeBlockIndex++;
    Node rightNode = createNode(node.getIsLeaf(), this->writeBlockIndex);
    this->writeBlockIndex++;

    node.insertEntry(entry);

    BTreeEntry ascendedEntry = node.retrieveMedianKeyEntry();

    std::pair<std::vector<BTreeEntry>, std::vector<BTreeEntry>> splitEntries = node.splitNode(); // split cleans the node
    leftNode.setEntries(splitEntries.first);
    leftNode.setParentPtr(0);
    leftNode.setIsFull(false);

    rightNode.setEntries(splitEntries.second);
    rightNode.setParentPtr(0);
    rightNode.setIsFull(false);
    
    //insert old ptr of ascended node to the most left  of the right node
    rightNode.insertChildPtr(ascendedEntry.getChildPtr());

    //insert ptr to right node to the ascended entry
    ascendedEntry.setChildPtr(rightNode.getBlockIndex());
    node.insertEntry(ascendedEntry);
    node.insertChildPtr(leftNode.getBlockIndex());
    rootCache = node;
   
  
    IndexFileManager.openFileStream();
    IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
    IndexFileManager.writeBlockToFile(leftNode.getBlockIndex(), leftNode.serialize().get());
    IndexFileManager.writeBlockToFile(rightNode.getBlockIndex(), rightNode.serialize().get());
    IndexFileManager.closeFileStream();

    updateParentPtrs();
   
}

Node IndexManager::findLeafNodeForKey(uint64_t key)
{
    Node currentNode = rootCache;
    while(!currentNode.getIsLeaf())
    {
        BTreeEntry prevEntry = currentNode.getEntries().front();
        if(!prevEntry.getChildPtr().has_value())
        {
            throw std::runtime_error("Node is not leaf, but has no child ptr");
        }
        // if node is not leaf, then it has to have child ptr
        for(auto entry : currentNode.getEntries())
        {
           if(entry.getKey().has_value())
           {
                if(key < entry.getKey().value())
                {
                    if(!prevEntry.getChildPtr().has_value())
                    {
                        throw std::runtime_error("Node is not leaf, but has no child ptr");
                    }
                    currentNode = getNode(prevEntry.getChildPtr().value());
                    break;
                }
           }
              prevEntry = entry;
        }
        currentNode = getNode(prevEntry.getChildPtr().value());
    }
    std::cout << "Found leaf node: " << currentNode.getBlockIndex() << std::endl;
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
    return Node(treeOrder, blockIndex, std::nullopt, isLeaf);
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
    std::cout<<std::endl<<"_____________________________________________________"<<std::endl;
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

bool IndexManager::checkIfCanCompensate(Node& node, BTreeEntry entry)
{
    if(!node.getParentPtr().has_value())
    {
        return false;
    }

    Node parentNode = getNode(node.getParentPtr().value());
    size_t index = -1;

    std::pair<std::optional<Node>,std::optional<Node>> siblings = findSiblings(parentNode, node.getBlockIndex());
    if(siblings.first.has_value())
    {
        if(siblings.first.value().getNumberOfKeys() < 2*treeOrder)
        {
            bool isLeftSibling = true;
            compensate(node,parentNode, siblings.first.value(), entry, isLeftSibling);
            return true;
        }
    }

    if(siblings.second.has_value())
    {
        if(siblings.second.value().getNumberOfKeys() < 2*treeOrder)
        {
            bool isLeftSibling = false;
            compensate(node, parentNode, siblings.second.value(), entry, isLeftSibling);
            return true;
        }
    }

    return false;
 
}

std::pair<std::optional<Node>,std::optional<Node>> IndexManager::findSiblings(const Node& parentNode, uint32_t blockIndex)
{
    size_t index = -1;
    for(size_t i = 0; i < parentNode.getEntries().size(); i++)
    {
        if(parentNode.getEntries()[i].getChildPtr().value() == blockIndex)
        {
            index = i;
            break;
        }
    }
    if(index == -1)
    {
        throw std::runtime_error("Node is not a child of its parent");
    }
    if(index == 0)
    {
        Node rightSibling = getNode(parentNode.getEntries()[index+1].getChildPtr().value());
        return std::make_pair(std::nullopt, rightSibling);
    }
    else if(index == parentNode.getEntries().size()-1)
    {
        Node leftSibling = getNode(parentNode.getEntries()[index-1].getChildPtr().value());
        return std::make_pair(leftSibling, std::nullopt);
    }
    else
    {
        Node leftSibling = getNode(parentNode.getEntries()[index-1].getChildPtr().value());
        Node rightSibling = getNode(parentNode.getEntries()[index+1].getChildPtr().value());
        return std::make_pair(leftSibling, rightSibling);
    }
}

void IndexManager::compensate(Node& node, Node& parentNode, Node& siblingNode, BTreeEntry entry, bool hasLeftSibling)
{

     size_t index = -1;
        for(size_t i = 0; i < parentNode.getEntries().size(); i++)
        {
            if(parentNode.getEntries()[i].getChildPtr().value() == node.getBlockIndex())
            {
                index = i;
                break;
            }
        }
    
    if(!hasLeftSibling)
    {
       
        index++; // its right slibing so i want to rotate with entry that has address of right sibling
        BTreeEntry currentRootEntry = parentNode.getEntries()[index]; // if this is right sibling, then its 100% node with key

        node.insertEntry(entry); // temporary overflow
        BTreeEntry entryToAscend = node.popRightMostEntryWithKey(); // this will become new root entry, 

        std::optional<uint32_t> ascendedEntryChildPtr = entryToAscend.getChildPtr();
        entryToAscend.setChildPtr(currentRootEntry.getChildPtr());
        parentNode.deleteEntryAtIndex(index); // delete currentRootEntry from parent
        parentNode.insertEntry(entryToAscend); // insert new root entry to parent


        std::optional<BTreeEntry> leftMostPtrOfRightSibling = siblingNode.popEntryWithoutKey();
        if(leftMostPtrOfRightSibling.has_value())
            currentRootEntry.setChildPtr(leftMostPtrOfRightSibling.value().getChildPtr());
        else
            currentRootEntry.setChildPtr(std::nullopt);
            
        siblingNode.insertEntry(currentRootEntry); // insert currentRootEntry to node
        siblingNode.insertChildPtr(ascendedEntryChildPtr); // insert left most ptr of right sibling to the right sibling

        //update parent ptrs
        

        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
        IndexFileManager.writeBlockToFile(parentNode.getBlockIndex(), parentNode.serialize().get());
        IndexFileManager.writeBlockToFile(siblingNode.getBlockIndex(), siblingNode.serialize().get());
        IndexFileManager.closeFileStream();

        if(parentNode.getBlockIndex() == 0)
        {
            rootCache = parentNode;
        }

    }

    else
    {
        BTreeEntry currentRootEntry = parentNode.getEntries()[index];
        if(currentRootEntry.getKey().has_value())
        {
            // if this is most left entry, then what???
        }
        
        node.insertEntry(entry); // temporary overflow
        BTreeEntry entryToAscend = node.popLeftMostEntryWithKey(); // this will become new root entry,

        std::optional<uint32_t> ascendedEntryChildPtr = entryToAscend.getChildPtr();
        entryToAscend.setChildPtr(currentRootEntry.getChildPtr());

        parentNode.deleteEntryAtIndex(index); // delete currentRootEntry from parent
        parentNode.insertEntry(entryToAscend); // insert new root entry to parent

        std::optional<BTreeEntry> leftMostPtrOfNode = node.popEntryWithoutKey();

        if(leftMostPtrOfNode.has_value())
            currentRootEntry.setChildPtr(leftMostPtrOfNode.value().getChildPtr());
        else
            currentRootEntry.setChildPtr(std::nullopt);

        node.insertChildPtr(ascendedEntryChildPtr); // insert left most ptr of node to the node
        siblingNode.insertEntry(currentRootEntry); // insert currentRootEntry to sibling

        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
        IndexFileManager.writeBlockToFile(parentNode.getBlockIndex(), parentNode.serialize().get());
        IndexFileManager.writeBlockToFile(siblingNode.getBlockIndex(), siblingNode.serialize().get());
        IndexFileManager.closeFileStream();

        if(parentNode.getBlockIndex() == 0)
        {
            rootCache = parentNode;
        }




    }
    
}

void IndexManager::updateParentPtrs()
{
    std::queue<Node> q;
    q.push(rootCache);
    while(!q.empty())
    {
        Node currentNode = q.front();
        q.pop();
        for(auto entry : currentNode.getEntries())
        {
            if(entry.getChildPtr().has_value())
            {
                Node childNode = getNode(entry.getChildPtr().value());
                childNode.setParentPtr(currentNode.getBlockIndex());
                IndexFileManager.openFileStream();
                IndexFileManager.writeBlockToFile(childNode.getBlockIndex(), childNode.serialize().get());
                IndexFileManager.closeFileStream();
                q.push(childNode);
            }
        }
    }
}