#pragma once 
#include <string>

namespace Commands {
    const std::string HELP = "help";
    const std::string QUIT = "quit";
    const std::string ADD = "add";
    const std::string CLEAR = "clear";
    const std::string SHOW = "print";
    const std::string HELP_FLAG = "--help";
    const std::string DELETE = "delete";
    const std::string SEARCH = "search";
    const std::string UPDATE = "update";

    const std::string INVALID_COMMAND = "Invalid command. For help, type: " + HELP;
    const std::string INVALID_FLAGS_ERROR = "Invalid flags. For help, type command name with " + HELP_FLAG;
   
}