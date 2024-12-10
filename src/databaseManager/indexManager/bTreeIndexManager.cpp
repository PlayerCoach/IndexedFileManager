#include "bTreeIndexManager.hpp"

IndexManager::IndexManager(std::string indexFilePath)
{
    this->indexFilePath = indexFilePath;
    this->IndexFileManager = FileManager(indexFilePath, indexPageSize, indexPageSize);

    IndexFileManager.openFileStream();
  
    Node root(treeOrder, 0);
    IndexFileManager.writeBlockToFile(this->writeBlockIndex, root.serialize().get());
    this->writeBlockIndex++;
    rootCache = root;

    IndexFileManager.closeFileStream();
}

std::string IndexManager::insertPreparation(DataEntry DataEntry, uint32_t databaseBlockIndex)
{
    BTreeEntry entry(DataEntry.getKey(), databaseBlockIndex, std::nullopt);
    if(entry.getKey().has_value() == false)
    {
        throw std::runtime_error("Key is not set");
    }


    std::pair<std::optional<Node>, bool> NodeAndFoundKeyPair = getNodeForKey(entry.getKey().value());
    if(NodeAndFoundKeyPair.second)
    {
        return "Key already exists";
    }
    if(!NodeAndFoundKeyPair.first.has_value())
    {
        return "Error while searching for place to insert key";
    }

    this->insertToLeaf(NodeAndFoundKeyPair.first.value(),entry);

}

    
void IndexManager::insertToLeaf(Node& nodeToInsert, BTreeEntry entry)
{
   
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
    this->writeNodeToFile(nodeToInsert.getBlockIndex(), nodeToInsert);
   
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
    if(!this->getParentNode(node).has_value())
    {
        splitRoot(node, entry);
        return;
    }

    Node parentNode = this->getParentNode(node).value();
    Node rightNode = createNode(node.getIsLeaf(), this->writeBlockIndex);
    // current node will be left node
    node.insertEntry(entry);
    BTreeEntry ascendedEntry = node.retrieveMedianKeyEntry();
    std::pair<std::vector<BTreeEntry>, std::vector<BTreeEntry>> splitEntries = node.splitNode(); // split cleans the node

    rightNode.setEntries(splitEntries.second);
    rightNode.insertChildPtr(ascendedEntry.getChildPtr());
    

    //insert ptr to right node to the ascended entry
    ascendedEntry.setChildPtr(rightNode.getBlockIndex());

    node.setEntries(splitEntries.first);
    IndexFileManager.openFileStream();
    IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
    IndexFileManager.writeBlockToFile(rightNode.getBlockIndex(), rightNode.serialize().get());
    IndexFileManager.closeFileStream();

    this->insertToNode(parentNode, ascendedEntry);

}

void IndexManager::splitRoot(Node& node, BTreeEntry entry)
{
    Node leftNode = createNode(node.getIsLeaf(), this->writeBlockIndex);
    Node rightNode = createNode(node.getIsLeaf(), this->writeBlockIndex);

    node.insertEntry(entry);

    BTreeEntry ascendedEntry = node.retrieveMedianKeyEntry();

    std::pair<std::vector<BTreeEntry>, std::vector<BTreeEntry>> splitEntries = node.splitNode(); // split cleans the node
    leftNode.setEntries(splitEntries.first);

    rightNode.setEntries(splitEntries.second);
    
    //insert old ptr of ascended node to the most left  of the right node
    rightNode.insertChildPtr(ascendedEntry.getChildPtr());

    //insert ptr to right node to the ascended entry
    ascendedEntry.setChildPtr(rightNode.getBlockIndex());
    node.insertEntry(ascendedEntry);
    node.insertChildPtr(leftNode.getBlockIndex());
    rootCache = node;
   
  
    this->writeNodeToFile(node.getBlockIndex(), node);
    this->writeNodeToFile(leftNode.getBlockIndex(), leftNode);
    this->writeNodeToFile(rightNode.getBlockIndex(), rightNode);
}


Node IndexManager::getNode(uint32_t blockIndex)
{
    IndexFileManager.openFileStream();
    auto data = IndexFileManager.readBlockFromFile(blockIndex);
    IndexFileManager.closeFileStream();
    return Node::deserialize(data.get(), treeOrder).value();
}

void IndexManager::deleteNode(uint32_t blockIndex)
{
   this->freeBlocks.push_back(blockIndex);
}

Node IndexManager::createNode(bool isLeaf, uint32_t blockIndex)
{
    Node node(treeOrder, blockIndex);
    this->writeBlockIndex++;
    return node;
}

void IndexManager::readBTree()
{
    if(rootCache.getNumberOfKeys() == 0)
    {
        std::cout<<"Empty tree"<<std::endl;
        return;
    }

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
    std::cout << "NumberOfKeys: " << node.getNumberOfKeys() << std::endl;
    
   for(auto entry : node.getEntries())
   {
    if(entry.getKey().has_value())
    {
        std::cout << "Key: " << entry.getKey().value() << " | ";
    }
    if(entry.getChildPtr().has_value())
    {
        std::cout << "ChildPtr: " << entry.getChildPtr().value() << " | ";
    }
       
   }

}

bool IndexManager::checkIfCanCompensate(Node& node, BTreeEntry entry)
{
    if(!this->getParentNode(node).has_value())
    {
        return false;
    }

    Node parentNode = this->getParentNode(node).value();
    size_t index = -1;

    std::pair<std::optional<Node>,std::optional<Node>> siblings = findSiblings(parentNode, node.getBlockIndex());
    if(siblings.first.has_value())
    {
        if(siblings.first.value().getNumberOfKeys() < 2*treeOrder)
        {
            bool isLeftSibling = true;
            compensateAfterInsertion(node,parentNode, siblings.first.value(), entry, isLeftSibling);
            return true;
        }
    }

    if(siblings.second.has_value())
    {
        if(siblings.second.value().getNumberOfKeys() < 2*treeOrder)
        {
            bool isLeftSibling = false;
            compensateAfterInsertion(node, parentNode, siblings.second.value(), entry, isLeftSibling);
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
        while(index < parentNode.getEntries().size() && parentNode.getEntries()[index].getChildPtr().value() == blockIndex)
        {
            index++;
        }
        Node rightSibling = getNode(parentNode.getEntries()[index].getChildPtr().value());
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

std::optional<Node> IndexManager::getParentNode(const Node& node)
{

   if(rootCache.getBlockIndex() == node.getBlockIndex())
    {
         return std::nullopt; // root has no parent
    }
   
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
                Node childNode = getNode(entry.getChildPtr().value()); // implement caching later on
                if(childNode.getBlockIndex() == node.getBlockIndex())
                {
                    return currentNode;
                }
                q.push(childNode);
            }
        }
       
    }

    return std::nullopt;
}

std::pair<std::optional<Node>, bool> IndexManager::getNodeForKey(uint64_t key)
{
    BTreeEntry wrapperForKey = BTreeEntry(key, std::nullopt, std::nullopt);
    Node currentNode = rootCache;

    while (true)
    {
        const auto& entries = currentNode.getEntries();
        size_t left = 0;
        size_t right = entries.size();

        // Perform binary search to find the appropriate child pointer.
        while (left < right)
        {
            size_t mid = left + (right - left) / 2;

            if(wrapperForKey == entries[mid])
            {
                return std::make_pair(currentNode, true);
            }
            else if(wrapperForKey < entries[mid])
            {
                right = mid;
            }
            else
            {
                left = mid + 1;
            }     
        }

        size_t childIndex = (left == 0) ? 0 : left - 1;
        const auto& selectedEntry = entries[childIndex];

        if (!selectedEntry.getChildPtr().has_value())
        {
            return std::make_pair(std::nullopt, false);
        }

        currentNode = getNode(selectedEntry.getChildPtr().value());

    } 

    return std::make_pair(currentNode, false);

}


std::string IndexManager::deleteKeyPreparation(uint64_t key)
{
    std::pair<std::optional<Node>, bool> nodeAndCheckIfKeyExistsPair = getNodeForKey(key);
    if(!nodeAndCheckIfKeyExistsPair.first.has_value() || !nodeAndCheckIfKeyExistsPair.second)
    {
        return "Key not found";
    }
    Node node = nodeAndCheckIfKeyExistsPair.first.value();
    this->deleteKey(node, key);
    return "Key deleted";
}

void IndexManager::deleteKey(Node& node, uint64_t key)
{

    if(node.getIsLeaf())
    {
        this->handleKeyRemoval(node, key);

    }
    else
    {

        if(handleMaxElementFromLeftSubtree(node, key))
            return;
   
        if(handleMinElementFromRightSubtree(node, key))
            return;
    }
    
    throw std::runtime_error("Key not found");
}

bool IndexManager::handleMaxElementFromLeftSubtree(Node& node, uint64_t key)
{
    std::pair<std::optional<BTreeEntry>, std::optional<Node>> maxElementFromLeftSubtree = findMaxElementFromLeftSubtree(node);
    if(maxElementFromLeftSubtree.first.has_value())
    {
        BTreeEntry maxEntry = maxElementFromLeftSubtree.first.value();
        BTreeEntry entryToDelete = node.getEntryWithKey(key).value();
        node.deleteEntryWithKey(key);
        maxEntry.setChildPtr(entryToDelete.getChildPtr());
        node.insertEntry(maxEntry);
        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
        IndexFileManager.closeFileStream();
        if(node.getBlockIndex() == 0)
        {
            rootCache = node;
        }
        this->handleKeyRemoval(maxElementFromLeftSubtree.second.value(), maxEntry.getKey().value());
        return true;
    }
    return false;
}

bool IndexManager::handleMinElementFromRightSubtree(Node& node, uint64_t key)
{
    std::pair<std::optional<BTreeEntry>, std::optional<Node>> minElementFromRightSubtree = findMinElementFromRightSubtree(node);
    if(minElementFromRightSubtree.first.has_value())
    {
        BTreeEntry minEntry = minElementFromRightSubtree.first.value();
        BTreeEntry entryToDelete = node.getEntryWithKey(key).value();
        node.deleteEntryWithKey(key);
        minEntry.setChildPtr(entryToDelete.getChildPtr());
        node.insertEntry(minEntry);
        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
        IndexFileManager.closeFileStream();
        if(node.getBlockIndex() == 0)
        {
            rootCache = node;
        }
        this->handleKeyRemoval(minElementFromRightSubtree.second.value(), minEntry.getKey().value());
        return true;
    }
    return false;
}

void IndexManager::handleKeyRemoval(Node& node, uint64_t key)
{
    if(node.getBlockIndex() == 0 )
    {
        node.deleteEntryWithKey(key);
        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
        IndexFileManager.closeFileStream();
        rootCache = node;
        return;

    }
    
    if(node.getNumberOfKeys() > treeOrder)
    {
        node.deleteEntryWithKey(key);
        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
        IndexFileManager.closeFileStream();
        return;
    }
    else
    {
        if(checkIfCanCompensateAfterDeletion(node, key))
            return;

        merge(node, key);
    }
}

std::pair<std::optional<BTreeEntry>, std::optional<Node>> IndexManager::findMaxElementFromLeftSubtree(Node& node)
{
    if(node.getIsLeaf())
    {
        throw std::runtime_error("Node is leaf");
    }

    Node currentNode = getNode(node.getEntries()[0].getChildPtr().value());
    while(!currentNode.getIsLeaf())
    {
        currentNode = getNode(currentNode.getEntries().back().getChildPtr().value());
    }

 
    BTreeEntry maxEntry = currentNode.getRightMostEntryWithKey();
    return std::make_pair(maxEntry, currentNode);
}

std::pair<std::optional<BTreeEntry>, std::optional<Node>> IndexManager::findMinElementFromRightSubtree(Node& node)
{
    if(node.getIsLeaf())
    {
        throw std::runtime_error("Node is leaf");
    }

    Node currentNode = getNode(node.getEntries().back().getChildPtr().value());
    while(!currentNode.getIsLeaf())
    {
        currentNode = getNode(currentNode.getEntries()[0].getChildPtr().value());
    }

    BTreeEntry minEntry = currentNode.getLeftMostEntryWithKey();

    return std::make_pair(minEntry, currentNode);

}

bool IndexManager::checkIfCanCompensateAfterDeletion(Node& node, uint64_t key)
{
    if(!this->getParentNode(node).has_value())
    {
        return false;
    }

    Node parentNode = this->getParentNode(node).value();
    size_t index = -1;

    std::pair<std::optional<Node>,std::optional<Node>> siblings = findSiblings(parentNode, node.getBlockIndex());
    if(siblings.first.has_value())
    {
        if(siblings.first.value().getNumberOfKeys() > treeOrder)
        {
            bool isLeftSibling = true;
            compensateAfterDeletion(node,parentNode, siblings.first.value(),  key, isLeftSibling);
            return true;
        }
    }

    if(siblings.second.has_value())
    {
        if(siblings.second.value().getNumberOfKeys() > treeOrder)
        {
            bool isLeftSibling = false;
            compensateAfterDeletion(node, parentNode, siblings.second.value(), key, isLeftSibling);
            return true;
        }
    }

    return false;
 
}

void IndexManager::merge(Node& node, uint64_t key)
{
    if(!this->getParentNode(node).has_value())
    {
        throw std::runtime_error("Node is root");
    }

    Node parentNode = this->getParentNode(node).value();
    size_t index = findChildIndex(parentNode, node.getBlockIndex());

    if(index == 0) // THIS IS ENTRY WITHOUT KEY
        this->mergeWithRightSibling(node, parentNode, key, index);

    else
        this->mergeWithLeftSibling(node, parentNode, key, index);



}

void IndexManager::mergeWithLeftSibling(Node& node, Node& parentNode, uint64_t key, size_t index)
{
    Node leftSibling = getNode(parentNode.getEntries()[index-1].getChildPtr().value());
    std::optional<BTreeEntry> childPtr = node.popEntryWithoutKey();
    std::vector<BTreeEntry> nodeEntries = node.getEntries();
    
    BTreeEntry entryToDescend = parentNode.getEntries()[index]; // this is entry with key
    parentNode.setEntryChildPtr(entryToDescend.getKey().value(), std::nullopt);

    
    entryToDescend.setChildPtr(childPtr.has_value() ? childPtr.value().getChildPtr() : std::nullopt);

    leftSibling.insertEntry(entryToDescend);

    for(auto entry : nodeEntries)
    {
        leftSibling.insertEntry(entry);
    }
    
    this->deleteNode(node.getBlockIndex());

    leftSibling.deleteEntryWithKey(key);

    this->updateTreeAfterMerge(leftSibling, parentNode, entryToDescend);

}

void IndexManager::mergeWithRightSibling(Node& node, Node& parentNode, uint64_t key, size_t index)
{
    Node rightSibling = getNode(parentNode.getEntries()[index+1].getChildPtr().value());
    std::optional<BTreeEntry> childPtr = rightSibling.popEntryWithoutKey();
    std::vector<BTreeEntry> siblingNodeEntries = rightSibling.getEntries();
    BTreeEntry entryToDescend = parentNode.getEntries()[index+1]; // this is entry with key
    parentNode.setEntryChildPtr(entryToDescend.getKey().value(), std::nullopt);

    entryToDescend.setChildPtr(childPtr.has_value() ? childPtr.value().getChildPtr() : std::nullopt);

    node.insertEntry(entryToDescend);
    for(auto entry : siblingNodeEntries)
    {
        node.insertEntry(entry);
    }
    
    this->deleteNode(rightSibling.getBlockIndex());
    node.deleteEntryWithKey(key);

    this->updateTreeAfterMerge(node, parentNode, entryToDescend);


}


void IndexManager::updateTreeAfterMerge(Node& target, Node& parentNode, BTreeEntry& entryToDescend)
{
    if(parentNode.getBlockIndex() == 0 && parentNode.getNumberOfKeys() == 1)
    {
        target.setSelfPtr(0);
        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(0, target.serialize().get()); // root is now leaf
        IndexFileManager.closeFileStream();
        rootCache = target;
        return;
    }

    if(parentNode.getBlockIndex() == 0)
    {
        parentNode.deleteEntryWithKey(entryToDescend.getKey().value());
        rootCache = parentNode;
        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(target.getBlockIndex(), target.serialize().get());
        IndexFileManager.writeBlockToFile(parentNode.getBlockIndex(), parentNode.serialize().get());
        IndexFileManager.closeFileStream();
        return;
    }

    writeNodeToFile(target.getBlockIndex(), target);
    writeNodeToFile(parentNode.getBlockIndex(), parentNode);
    

    this->handleKeyRemoval(parentNode, entryToDescend.getKey().value());

}


size_t IndexManager::findChildIndex(const Node& node, uint32_t childBlockIndex) {
    for (size_t i = 0; i < node.getEntries().size(); i++) {
        if (node.getEntries()[i].getChildPtr().value() == childBlockIndex) {
            return i;
        }
    }

    throw std::runtime_error("Child index not found");
}


void IndexManager::compensateHelper(Node& node, Node& parentNode, Node& siblingNode, std::optional<BTreeEntry> entry, uint64_t key, bool hasLeftSibling, bool isDeletion) {
    
  
    size_t index = findChildIndex(parentNode, node.getBlockIndex());

    if (!hasLeftSibling) {
        index++; // right sibling rotation
        BTreeEntry currentRootEntry = parentNode.getEntries()[index];

        if (!isDeletion && entry.has_value()) {
            node.insertEntry(entry.value()); // temporary overflow for insertion
        }

        BTreeEntry entryToAscend = isDeletion ? siblingNode.popLeftMostEntryWithKey() : node.popRightMostEntryWithKey();
        std::optional<uint32_t> ascendedEntryChildPtr = entryToAscend.getChildPtr();
        entryToAscend.setChildPtr(currentRootEntry.getChildPtr());

        parentNode.deleteEntryAtIndex(index);
        parentNode.insertEntry(entryToAscend);

        std::optional<BTreeEntry> leftMostPtrOfRightSibling = siblingNode.popEntryWithoutKey();
        currentRootEntry.setChildPtr(leftMostPtrOfRightSibling.has_value() ? leftMostPtrOfRightSibling.value().getChildPtr() : std::nullopt);

        if (isDeletion) {
            node.insertEntry(currentRootEntry);
            node.deleteEntryWithKey(key);
        } else {
            siblingNode.insertEntry(currentRootEntry);
            siblingNode.insertChildPtr(ascendedEntryChildPtr);
        }

    } else {
        BTreeEntry currentRootEntry = parentNode.getEntries()[index];

        if (!isDeletion && entry.has_value()) {
            node.insertEntry(entry.value()); // temporary overflow for insertion
        }

        BTreeEntry entryToAscend = isDeletion ? siblingNode.popRightMostEntryWithKey() : node.popLeftMostEntryWithKey();
        std::optional<uint32_t> ascendedEntryChildPtr = entryToAscend.getChildPtr();
        entryToAscend.setChildPtr(currentRootEntry.getChildPtr());

        parentNode.deleteEntryAtIndex(index);
        parentNode.insertEntry(entryToAscend);

        std::optional<BTreeEntry> leftMostPtrOfNode = isDeletion ? siblingNode.popEntryWithoutKey() : node.popEntryWithoutKey();
        currentRootEntry.setChildPtr(leftMostPtrOfNode.has_value() ? leftMostPtrOfNode.value().getChildPtr() : std::nullopt);

        if (isDeletion) {
            node.insertEntry(currentRootEntry);
            node.deleteEntryWithKey(key);
        } else {
            node.insertChildPtr(ascendedEntryChildPtr);
            siblingNode.insertEntry(currentRootEntry);
        }
    }

    // Update the file streams
    this->writeNodeToFile(node.getBlockIndex(), node);
    this->writeNodeToFile(parentNode.getBlockIndex(), parentNode);
    this->writeNodeToFile(siblingNode.getBlockIndex(), siblingNode);

    if (parentNode.getBlockIndex() == 0) {
        rootCache = parentNode;
    }
}

void IndexManager::compensateAfterDeletion(Node& node, Node& parentNode, Node& siblingNode, uint64_t key, bool hasLeftSibling) {
    compensateHelper(node, parentNode, siblingNode, std::nullopt, key, hasLeftSibling, true);
}

void IndexManager::compensateAfterInsertion(Node& node, Node& parentNode, Node& siblingNode, BTreeEntry entry, bool hasLeftSibling) {
    compensateHelper(node, parentNode, siblingNode, entry, 0, hasLeftSibling, false);
}

void IndexManager::writeNodeToFile(uint32_t blockIndex, Node& node)
{
    IndexFileManager.openFileStream();
    IndexFileManager.writeBlockToFile(blockIndex, node.serialize().get());
    IndexFileManager.closeFileStream();
}





        
       

     

