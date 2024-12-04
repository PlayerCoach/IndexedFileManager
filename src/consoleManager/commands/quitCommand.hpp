#include "ICommand.hpp"
#include "commands.hpp"
#include <iostream>
#include <unordered_map>


class QuitCommand : public Command 
{
    private:

    std::string getDescription() override {
        return "Exits the program";
    } 
    

    public:

    QuitCommand() {
        this->flags = {
            {Commands::HELP_FLAG, false},
        };
    }

    void execute(std::vector<std::string> tokens) override
    {
        if(!this->setFlags(tokens)) return;

        if (this->flags[Commands::HELP_FLAG]) {
            this->printHelp();
            return;
        }

        exit(0);
    }
    
   
};
