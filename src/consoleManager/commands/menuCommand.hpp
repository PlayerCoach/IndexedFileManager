#pragma once
#include "ICommand.hpp"
#include "commands.hpp"
#include <iostream>
#include <string>


class MenuCommand : public Command {
private:

    const std::string MENU =
        "\n========================================\n"
        "                MAN\n"
        "========================================\n"
        "1.) " + Commands:: HELP + " -> prints this menu \n"
        "2.) " + Commands::ADD + "\n"
        "3.) " + Commands::CLEAR + "\n"
        "4.) " + Commands::SHOW + "\n"
        "5.) " + Commands::QUIT + "\n"
        "6.) " + Commands::SEARCH + "\n"
        "7.) " + Commands::DELETE + "\n"
        "8.) " + Commands::UPDATE + "\n"
        "9.) " + "read -> reads commands from a file\n"
        "To check the flags of a command, type the command name followed by " + Commands::HELP_FLAG + "\n"
        
        "\n========================================\n";

    std::string getDescription() override {
        return "Prints the menu";
    }

    public:
    MenuCommand() {
        this->flags = {
            {Commands::HELP_FLAG, false},
        };
    }
    void execute(std::vector<std::string> tokens) override 
    {
        if(!this->setFlags(tokens)) return;

        if (this->flags[Commands::HELP_FLAG])
        {
            this->printHelp();
            return;
        }
        
        std::cout << MENU << std::endl;
    }
};
