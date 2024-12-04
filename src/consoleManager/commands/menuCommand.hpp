#pragma once
#include "ICommand.hpp"
#include "commands.hpp"
#include <iostream>


class MenuCommand : public ICommand {
private:
    const std::string MENU =
        "\n========================================\n"
        "                MAN\n"
        "========================================\n"
        "1.) " + Commands:: HELP + " -> prints this menu \n"
        "2.) " + Commands::ADD + "\n"
        "3.) " + Commands::CLEAR + "\n"
        "4.) " + Commands::SHOW + "\n"
        "5.) " + Commands::QUIT +
        "\n========================================\n";

public:
    void execute(std::vector<std::string> tokens) override 
    {
        if(!tokens.empty())
        {
            std::cout << Commands::INVALID_COMMAND << std::endl;
            return;
        }


        std::cout << MENU << std::endl;
    }

    void printHelp() override
    {
        return;
    }
};
