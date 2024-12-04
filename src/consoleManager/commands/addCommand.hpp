// #include "ICommand.hpp"
// #include "commands.hpp"
// #include <iostream>
// #include <unordered_map>

// class AddCommand :: public Command {
//     private:
//            const std::string ADD_RANDOM = "-r";
//         const std::string ADD_USER = "-u";
//     this->flags = {
//         {Commands::HELP, false},
//     };
    
//     public:
//     void execute(std::vector<std::string> tokens) override
//     {
//         if(!tokens.empty())
//         {
//             std::cout << Commands::INVALID_COMMAND << std::endl;
//             return;
//         }
//         exit(0);
//     }
//     void printHelp() override {}
// }