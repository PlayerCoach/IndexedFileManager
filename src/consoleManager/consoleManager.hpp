#include <iostream>
#include <unordered_map>
#include <string>
#include <functional>   
#include <sstream>
#include "dataEntry.hpp"

class ConsoleManager
{
    public:
    static const std::string HELP;
    static const std::string QUIT;
    static const std::string ADD;
        static const std::string ADD_RANDOM;
        static const std::string ADD_USER;
    static const std::string CLEAR;
    static const std::string SHOW;
    static const std::string MENU;
    static const std::string INVALID_COMMAND;
    
    private:
    std::unordered_map<std::string, std::function<void(std::vector<std::string>&)>> commands;

    public:
    ConsoleManager();
    void getUserInput();
    void parseUserInput(const std::string& input);
    void initCommands();
    bool checkIfTokensEmpty(std::vector<std::string> tokens);
    void printMenuCommand(std::vector<std::string> tokens = {});
    void clearScreenCommand(std::vector<std::string> tokens = {});         
    void addCommand(std::vector<std::string> tokens);
    void addRandom(int count);
    void userAddCommand(int count);
    void exitProgramCommand(std::vector<std::string> tokens);

};
