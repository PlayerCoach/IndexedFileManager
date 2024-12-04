#pragma once 
#include "ICommand.hpp"
#include "menuCommand.hpp"
#include "quitCommand.hpp"
#include "clearCommand.hpp"
#include "addCommand.hpp"
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
        }

        std::unique_ptr<Command> createCommand(const std::string& commandName) {
            if (commandMap.find(commandName) != commandMap.end()) {
                return commandMap[commandName]();
            }
            return nullptr;
        }


};