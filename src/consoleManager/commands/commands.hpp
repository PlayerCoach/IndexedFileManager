#pragma once 
#include <string>

namespace Commands {
    const std::string HELP = "help";
    const std::string QUIT = "quit";
    const std::string ADD = "add";
    const std::string CLEAR = "clear";
    const std::string SHOW = "show";
    const std::string HELP_FLAG = "--help";

    const std::string INVALID_COMMAND = "Invalid command. For help, type: " + HELP;
    const std::string INVALID_FLAGS_ERROR = "Invalid flags. For help, type command name with " + HELP_FLAG;
   
}