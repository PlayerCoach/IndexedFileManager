#include "keyGen.hpp"

KeyGen::KeyGen() : gen(std::random_device{}()), dist(0, UINT64_MAX) {}

int64_t KeyGen::generateKey()
{
    uint64_t key;

    do {
        key = dist(gen);
    } while (keys.find(key) != keys.end());  // Ensure uniqueness

    keys.insert(key);
    return key;
}

void KeyGen::removeKey(uint64_t key)
{
    try
    {
        keys.erase(key);

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void KeyGen::insertKey(uint64_t key)
{
    try
    {
        if(!keyExists(key))
        {
            keys.insert(key);
            return;
        }

        throw std::invalid_argument("Key already exists");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

const std::unordered_set<uint64_t>& KeyGen::getKeys() const
{
    return keys;
}

bool KeyGen::keyExists(uint64_t key)
{
    return keys.find(key) != keys.end();
}

void KeyGen::clearKeys()
{
    keys.clear();
}



