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


