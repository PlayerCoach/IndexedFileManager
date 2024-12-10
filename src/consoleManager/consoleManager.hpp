#include <iostream>
#include <unordered_map>
#include <string>
#include <functional>   
#include <sstream>
#include "commandFactory.hpp"
#include "commands.hpp"

class ConsoleManager
{
    private:
    CommandFactory commands;

    public:
    ConsoleManager();
    void getUserInput();
    void parseInput(const std::string& input);
    void readCommandsFromFile(const std::string& fileName);
    

};
