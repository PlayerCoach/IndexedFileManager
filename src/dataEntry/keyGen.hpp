#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

class KeyGen
{
    private:
        std::mt19937 gen;
        std::uniform_int_distribution<uint64_t> dist;

        std::unordered_set<uint64_t> keys;
    
    public:
        KeyGen();
        int64_t generateKey();
        void removeKey(uint64_t key);
        void insertKey(uint64_t key);
        const std::unordered_set<uint64_t>& getKeys() const;
        bool keyExists(uint64_t key);
        void clearKeys();

    
};