#pragma once 
#include "ICommand.hpp"
#include "menuCommand.hpp"
#include "commands.hpp"
#include <unordered_map>
#include <functional>
#include <memory>


class CommandFactory 
{

    private:
    std::unordered_map<std::string, std::function<std::unique_ptr<ICommand>()>> commandMap;

    public:
        CommandFactory() {
            commandMap[Commands::HELP] = []() { return std::make_unique<MenuCommand>(); };
            //commandMap["quit"] = []() { return std::make_unique<QuitCommand>(); };
        }

        std::unique_ptr<ICommand> createCommand(const std::string& commandName) {
            if (commandMap.find(commandName) != commandMap.end()) {
                return commandMap[commandName]();
            }
            return nullptr;
        }


};