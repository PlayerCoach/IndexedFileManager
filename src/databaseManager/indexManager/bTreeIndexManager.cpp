#include "bTreeIndexManager.hpp"

IndexManager::IndexManager(std::string indexFilePath)
{
    this->indexFilePath = indexFilePath;
    this->IndexFileManager = FileManager(indexFilePath, indexPageSize, indexPageSize);

    IndexFileManager.openFileStream();
    if (IndexFileManager.checkIfFileIsEmpty())
    {
        Node root(treeOrder, 0, true);
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
    if(!this->getParentNode(node).has_value())
    {
        splitRoot(node, entry);
        return;
    }

    Node parentNode = this->getParentNode(node).value();
    Node rightNode = createNode(node.getIsLeaf(), this->writeBlockIndex);
    this->writeBlockIndex++;
    // current node will be left node
    node.insertEntry(entry);
    BTreeEntry ascendedEntry = node.retrieveMedianKeyEntry();
    std::pair<std::vector<BTreeEntry>, std::vector<BTreeEntry>> splitEntries = node.splitNode(); // split cleans the node
    node.setIsLeaf(rightNode.getIsLeaf());

    rightNode.setEntries(splitEntries.second);
    rightNode.insertChildPtr(ascendedEntry.getChildPtr());
    
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
    leftNode.setIsFull(false);

    rightNode.setEntries(splitEntries.second);
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
   
}

Node IndexManager::findLeafNodeForKey(uint64_t key)
{
    BTreeEntry wrapperForKey(key, std::nullopt, std::nullopt);
    Node currentNode = rootCache;

    while (!currentNode.getIsLeaf())
    {
        const auto& entries = currentNode.getEntries();
        size_t left = 0;
        size_t right = entries.size();

        // Perform binary search to find the appropriate child pointer.
        while (left < right)
        {
            size_t mid = left + (right - left) / 2;

            if (wrapperForKey < entries[mid])
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
            throw std::runtime_error("Node is not leaf, but has no child ptr");
        }

        currentNode = getNode(selectedEntry.getChildPtr().value());
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

void IndexManager::deleteNode(uint32_t blockIndex)
{
   this->freeBlocks.push_back(blockIndex);
}

Node IndexManager::createNode(bool isLeaf, uint32_t blockIndex)
{
    return Node(treeOrder, blockIndex, isLeaf);
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

std::optional<Node> IndexManager::findNodeWithKey(uint64_t key)
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
                return currentNode;
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
            return std::nullopt;
        }

        currentNode = getNode(selectedEntry.getChildPtr().value());

    } 

}


std::string IndexManager::deleteKeyPreparation(uint64_t key)
{
    std::optional<Node> nodeWithKey = findNodeWithKey(key);
    if(!nodeWithKey.has_value())
    {
        return "Key not found";
    }
    Node node = nodeWithKey.value();
    this->deleteKey(node, key);
    return "Key deleted";
}

void IndexManager::deleteKey(Node& node, uint64_t key)
{

    if(node.getIsLeaf())
    {
        this->deleteKeyFromLeaf(node, key);

    }
    else
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
            this->deleteKeyFromLeaf(maxElementFromLeftSubtree.second.value(), maxEntry.getKey().value());
            return;
        }

        std::pair<std::optional<BTreeEntry>, std::optional<Node>>  minElementFromRightSubtree = findMinElementFromRightSubtree(node);
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
            this->deleteKeyFromLeaf(minElementFromRightSubtree.second.value(), minEntry.getKey().value());
            return;
        }
    }
    
    return;

}

void IndexManager::deleteKeyFromLeaf(Node& node, uint64_t key)
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

    // if(currentNode.getNumberOfKeys() - 1 < treeOrder)
    // {
    //     return std::make_pair(std::nullopt, std::nullopt);
    // }
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

void IndexManager::compensateAfterDeletion(Node& node, Node& parentNode, Node& siblingNode, uint64_t key,  bool hasLeftSibling)
{
    //CHANGE THIS FUNCTION 
    //THIS IS NOT LEAF NODE FOR SURE
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

        BTreeEntry entryToAscend = siblingNode.popLeftMostEntryWithKey(); // this will become new root entry,

        std::optional<uint32_t> ascendedEntryChildPtr = entryToAscend.getChildPtr();
        entryToAscend.setChildPtr(currentRootEntry.getChildPtr());
        parentNode.deleteEntryAtIndex(index); // delete currentRootEntry from parent
        parentNode.insertEntry(entryToAscend); // insert new root entry to parent

        std::optional<BTreeEntry> leftMostPtrOfRightSibling = siblingNode.popEntryWithoutKey();
        if(leftMostPtrOfRightSibling.has_value())
            currentRootEntry.setChildPtr(leftMostPtrOfRightSibling.value().getChildPtr());
        else
            currentRootEntry.setChildPtr(std::nullopt);
       
       
        node.insertEntry(currentRootEntry); // insert currentRootEntry to node
        node.deleteEntryWithKey(key); // delete key from node

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
        
        BTreeEntry entryToAscend = siblingNode.popRightMostEntryWithKey(); // this will become new root entry,

        std::optional<uint32_t> ascendedEntryChildPtr = entryToAscend.getChildPtr();
        entryToAscend.setChildPtr(currentRootEntry.getChildPtr());

        parentNode.deleteEntryAtIndex(index); // delete currentRootEntry from parent
        parentNode.insertEntry(entryToAscend); // insert new root entry to parent

        std::optional<BTreeEntry> leftMostPtrOfNode = siblingNode.popEntryWithoutKey();

        if(leftMostPtrOfNode.has_value())
            currentRootEntry.setChildPtr(leftMostPtrOfNode.value().getChildPtr());
        else
            currentRootEntry.setChildPtr(std::nullopt);

        node.insertEntry(currentRootEntry); // insert currentRootEntry to sibling
        node.deleteEntryWithKey(key); // delete key from node

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


void IndexManager::merge(Node& node, uint64_t key)
{
    if(!this->getParentNode(node).has_value())
    {
        throw std::runtime_error("Node is root");
    }

    Node parentNode = this->getParentNode(node).value();
    size_t index = -1;
    for(size_t i = 0; i < parentNode.getEntries().size(); i++)
    {
        if(parentNode.getEntries()[i].getChildPtr().value() == node.getBlockIndex())
        {
            index = i;
            break;
        }
    }

    if(index == 0) // THIS IS ENTRY WITHOUT KEY
    {
        
        Node rightSibling = getNode(parentNode.getEntries()[index+1].getChildPtr().value());
        std::optional<BTreeEntry> childPtr = rightSibling.popEntryWithoutKey();
        std::vector<BTreeEntry> rightSiblingEntries = rightSibling.getEntries();
        BTreeEntry entryToDescend = parentNode.getEntries()[index + 1]; // this is entry with key
        parentNode.setEntryChildPtr(entryToDescend.getKey().value(), std::nullopt); // so it can you use it as child ptr
        if(childPtr.has_value())
            entryToDescend.setChildPtr(childPtr.value().getChildPtr());
        else
            entryToDescend.setChildPtr(std::nullopt);

        node.insertEntry(entryToDescend);
        for(auto entry : rightSiblingEntries)
        {
            node.insertEntry(entry);
        }
        
        this->deleteNode(rightSibling.getBlockIndex());
        node.deleteEntryWithKey(key);

        if(parentNode.getBlockIndex() == 0 && parentNode.getNumberOfKeys() == 1)
        {
            node.setSelfPtr(0);
            node.setIsLeaf(true);
            IndexFileManager.openFileStream();
            IndexFileManager.writeBlockToFile(0, node.serialize().get()); // root is now leaf
            IndexFileManager.closeFileStream();
            rootCache = node;
            return;


        }

        if(parentNode.getBlockIndex() == 0)
        {
            parentNode.deleteEntryWithKey(entryToDescend.getKey().value());
            rootCache = parentNode;
            IndexFileManager.openFileStream();
            IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
            IndexFileManager.writeBlockToFile(parentNode.getBlockIndex(), parentNode.serialize().get());
            IndexFileManager.closeFileStream();
            return;
        }

        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(node.getBlockIndex(), node.serialize().get());
        IndexFileManager.writeBlockToFile(parentNode.getBlockIndex(), parentNode.serialize().get());
        IndexFileManager.closeFileStream();

        this->deleteKeyFromLeaf(parentNode, entryToDescend.getKey().value());
   
    }

    else
    {
        Node leftSibling = getNode(parentNode.getEntries()[index-1].getChildPtr().value());
        std::optional<BTreeEntry> childPtr = node.popEntryWithoutKey();
        std::vector<BTreeEntry> nodeEntries = node.getEntries();
        BTreeEntry entryToDescend = parentNode.getEntries()[index]; // this is entry with key
        parentNode.setEntryChildPtr(entryToDescend.getKey().value(), std::nullopt);

        if(childPtr.has_value())
            entryToDescend.setChildPtr(childPtr.value().getChildPtr());
        else
            entryToDescend.setChildPtr(std::nullopt);

        leftSibling.insertEntry(entryToDescend);
        for(auto entry : nodeEntries)
        {
            leftSibling.insertEntry(entry);
        }
        
        this->deleteNode(node.getBlockIndex());
        leftSibling.deleteEntryWithKey(key);

        

        if(parentNode.getBlockIndex() == 0 && parentNode.getNumberOfKeys() == 1)
        {
            leftSibling.setSelfPtr(0);
            leftSibling.setIsLeaf(true);
            IndexFileManager.openFileStream();
            IndexFileManager.writeBlockToFile(0, leftSibling.serialize().get()); // root is now leaf
            IndexFileManager.closeFileStream();

            rootCache = leftSibling;
            return;
        }

        if(parentNode.getBlockIndex() == 0)
        {
            parentNode.deleteEntryWithKey(entryToDescend.getKey().value());
            rootCache = parentNode;
            IndexFileManager.openFileStream();
            IndexFileManager.writeBlockToFile(leftSibling.getBlockIndex(), leftSibling.serialize().get());
            IndexFileManager.writeBlockToFile(parentNode.getBlockIndex(), parentNode.serialize().get());
            IndexFileManager.closeFileStream();
            return;
        }

        IndexFileManager.openFileStream();
        IndexFileManager.writeBlockToFile(leftSibling.getBlockIndex(), leftSibling.serialize().get());
        IndexFileManager.writeBlockToFile(parentNode.getBlockIndex(), parentNode.serialize().get());
        IndexFileManager.closeFileStream();

        this->deleteKeyFromLeaf(parentNode, entryToDescend.getKey().value());



    }


}




        
       

     

