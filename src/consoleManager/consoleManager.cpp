#include "consoleManager.hpp"


const std::string ConsoleManager::HELP = "help";
const std::string ConsoleManager::QUIT = "quit";
const std::string ConsoleManager::ADD = "add";
    const std::string ConsoleManager::ADD_RANDOM = "-r";
    const std::string ConsoleManager::ADD_USER = "-u";
const std::string ConsoleManager::CLEAR = "clear";
const std::string ConsoleManager::SHOW = "show";
const std::string ConsoleManager::MENU = 
  "\n========================================\n"
    "                MAN\n"
    "========================================\n"
    "1.) " + ConsoleManager::HELP + "\n"
    "2.) " + ConsoleManager::ADD + 
        "+ count[int] +" + ConsoleManager::ADD_RANDOM + " (random)" + ConsoleManager::ADD_USER + "(user specific) \n"
    "3.) " + ConsoleManager::CLEAR + "\n"
    "4.) " + ConsoleManager::SHOW + "\n"
    "5.) " + ConsoleManager::QUIT +
    "\n========================================\n";

const std::string ConsoleManager::INVALID_COMMAND = "Invalid command for help type" + ConsoleManager::HELP + "\n";


ConsoleManager::ConsoleManager()
{
    this->initCommands();

}

void ConsoleManager::getUserInput()
{
    this->clearScreenCommand({});
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
    std::string command = tokens[0];
    tokens.erase(tokens.begin());

    if(this->commands.find(command) != this->commands.end())
    {
        this->commands[command](tokens);
    }
    else
    {
        std::cout << "Invalid command" << std::endl;
    }
}

void ConsoleManager::printMenuCommand(std::vector<std::string> tokens)
{
    if(!this->checkIfTokensEmpty(tokens))
    {
        std::cout << INVALID_COMMAND << std::endl;
        return;
    }
    std::cout << MENU << std::endl;
}

void ConsoleManager::clearScreenCommand(std::vector<std::string> tokens)
{
    
    if(!this->checkIfTokensEmpty(tokens))
    {
        std::cout << INVALID_COMMAND << std::endl;
        return;
    }

    #ifdef _WIN32
	system("cls");
    #else
	system("clear");
    #endif

    this->printMenuCommand();
}

bool ConsoleManager::checkIfTokensEmpty(std::vector<std::string> tokens)
{
    if(tokens.size() == 0)
    {
        return true;
    }
    return false;
}

void ConsoleManager::exitProgramCommand(std::vector<std::string> tokens)
{
    if(!this->checkIfTokensEmpty(tokens))
    {
        std::cout << INVALID_COMMAND << std::endl;
        return;
    }
    exit(0);
}

void ConsoleManager::addCommand(std::vector<std::string> tokens)
{
    int count = 0;
    try{count = stoi(tokens[0]); }
    catch(const std::exception& e){ std::cerr << e.what() << '\n'; return; }

    tokens.erase(tokens.begin());

    if(tokens.size() == 0)
    {
        std::cout << INVALID_COMMAND << std::endl;
        return;
    }

    if(tokens[0] == ConsoleManager::ADD_RANDOM)
    {
        this->addRandom(count);
    }
    else if(tokens[0] == ConsoleManager::ADD_USER)
    {
        this->userAddCommand(count);
    }
    else
    {
        std::cout << INVALID_COMMAND << std::endl;
    }
}

void ConsoleManager::addRandom(int count)
{
    for(int i = 0; i < count; i++)
    {
        DataEntry dataEntry;
        std::cout << dataEntry << std::endl;
    }
}

void ConsoleManager::userAddCommand(int count)
{
   return;
}

void ConsoleManager::initCommands()
{
    this->commands[ConsoleManager::HELP] = [this](std::vector<std::string> tokens){ this->printMenuCommand(tokens); };
    this->commands[ConsoleManager::QUIT] = [this](std::vector<std::string> tokens){this->exitProgramCommand(tokens);};
    this->commands[ConsoleManager::ADD] = [this](std::vector<std::string> tokens ){this->addCommand(tokens);};
    this->commands[ConsoleManager::CLEAR] = [this](std::vector<std::string> tokens ){this->clearScreenCommand(tokens);};
    this->commands[ConsoleManager::SHOW] = [this](std::vector<std::string> tokens ){std::cout << "SHOW command" << std::endl;};
}

