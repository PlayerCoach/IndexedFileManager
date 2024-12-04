#pragma once 
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include "commands.hpp"

class Command 
{
    protected:

    virtual std::string getDescription() { return "No description available";}

    std::unordered_map<std::string, bool> flags;
    
    virtual void printHelp() 
    {
        std::cout << getDescription() << std::endl;
    }

    virtual bool setFlags(std::vector<std::string> tokens) {

        for (const auto& token : tokens) {
            if (flags.find(token) != flags.end()) {
                flags[token] = true;
            }
            else
            {
                std::cout << Commands::INVALID_FLAGS_ERROR << std::endl;
                return false;
            }
        }
        return true;
    }
    
    virtual bool checkFlagsConflict() {return false;} // abstract method not every command needs to check flags conflict
    
    public:

    virtual void execute(std::vector<std::string> tokens) = 0;
    virtual ~Command() = default;
};
