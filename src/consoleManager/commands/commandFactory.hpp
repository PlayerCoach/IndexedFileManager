#pragma once 
#include "ICommand.hpp"
#include "menuCommand.hpp"
#include "quitCommand.hpp"
#include "clearCommand.hpp"
#include "addCommand.hpp"
#include "deleteCommand.hpp"
#include "searchCommand.hpp"
#include "printCommand.hpp"
#include "updateCommand.hpp"
#include "commands.hpp"
#include <unordered_map>
#include <functional>
#include <memory>


class CommandFactory 
{

    private:
    std::unordered_map<std::string, std::function<std::unique_ptr<Command>()>> commandMap;

    public:
        CommandFactory() {
            commandMap[Commands::HELP] = []() { return std::make_unique<MenuCommand>(); };
            commandMap[Commands::QUIT] = []() { return std::make_unique<QuitCommand>(); };
            commandMap[Commands::CLEAR] = []() { return std::make_unique<ClearCommand>(); };
            commandMap[Commands::ADD] = []() { return std::make_unique<AddCommand>(); };
            commandMap[Commands::DELETE] = []() { return std::make_unique<DeleteCommand>(); };
            commandMap[Commands::SEARCH] = []() { return std::make_unique<SearchCommand>(); };
            commandMap[Commands::SHOW] = []() { return std::make_unique<PrintCommand>(); };
            commandMap[Commands::UPDATE] = []() { return std::make_unique<UpdateCommand>(); };
        }

        std::unique_ptr<Command> createCommand(const std::string& commandName) {
            if (commandMap.find(commandName) != commandMap.end()) {
                return commandMap[commandName]();
            }
            return nullptr;
        }


};