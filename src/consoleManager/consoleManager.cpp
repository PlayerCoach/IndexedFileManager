#include "consoleManager.hpp"

ConsoleManager::ConsoleManager(){}

void ConsoleManager::getUserInput()
{
    this->clearScreen();
    std::string userInput;
    
    while(true)
    {
        std::cout << "Enter command: ";
        std::getline(std::cin, userInput);
        this->parseUserInput(userInput);
       
    }
}

void ConsoleManager::parseUserInput(const std::string& input)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(input);
    while (std::getline(tokenStream, token, ' '))
    {
        tokens.push_back(token);
    }
    std::string commandName = tokens[0];
    tokens.erase(tokens.begin());

    std::unique_ptr<ICommand> command = this->commands.createCommand(commandName);

    if (command) {
        command->execute(tokens);
    } else {
        std::cout << "Invalid command" << std::endl;
    }
}

void ConsoleManager::clearScreen()
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


