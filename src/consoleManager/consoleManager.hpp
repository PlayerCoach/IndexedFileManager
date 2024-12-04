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
    void parseUserInput(const std::string& input);

};
