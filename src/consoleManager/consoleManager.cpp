#include "consoleManager.hpp"

ConsoleManager::ConsoleManager(){}

void ConsoleManager::getUserInput()
{
    this->commands.createCommand(Commands::CLEAR)->execute({});
    this->commands.createCommand(Commands::HELP)->execute({});
    std::string userInput;
    
    while(true)
    {
        std::cout << "> ";
        std::getline(std::cin, userInput);
        this->parseInput(userInput);
        std::cout << std::endl;
       
    }
}

void ConsoleManager::parseInput(const std::string& input)
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

    if(commandName == "read" && tokens.size() == 1)
    {
        this->readCommandsFromFile(tokens[0]);
        return;
    }

    std::unique_ptr<Command> command = this->commands.createCommand(commandName);

    if (command) {
        command->execute(tokens);
    } else {
        std::cout << "Invalid command" << std::endl;
    }
}

void ConsoleManager::readCommandsFromFile(const std::string& fileName)
{
    std::ifstream file(fileName);
    std::string line;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            // Trim leading/trailing whitespace and check if the line is not empty
            line.erase(line.find_last_not_of(" \t\n\r") + 1); // Trim trailing whitespace
            line.erase(0, line.find_first_not_of(" \t\n\r")); // Trim leading whitespace

            if (!line.empty())
            {
                this->parseInput(line);
            }
        }
        file.close();
    }
    else
    {
        std::cout << "Unable to open file" << std::endl;
    }
}




