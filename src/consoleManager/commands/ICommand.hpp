#pragma once 
#include <vector>
#include <string>

class ICommand 
{
    protected:
    virtual void printHelp() = 0;

    public:
    virtual void execute(std::vector<std::string> tokens) = 0;
    virtual ~ICommand() = default;
};
