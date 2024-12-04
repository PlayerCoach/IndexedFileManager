#pragma once 
#include <string>

namespace Commands {
    const std::string HELP = "help";
    const std::string QUIT = "quit";
    const std::string ADD = "add";
        const std::string ADD_RANDOM = "-r";
        const std::string ADD_USER = "-u";
    const std::string CLEAR = "clear";
    const std::string SHOW = "show";

    const std::string INVALID_COMMAND = "Invalid command. For help, type: " + HELP;
   
}