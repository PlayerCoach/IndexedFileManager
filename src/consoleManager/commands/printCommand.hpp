#include "ICommand.hpp"
#include "commands.hpp"
#include <iostream>
#include <unordered_map>
#include "databaseManager.hpp"


class PrintCommand : public Command 
{
    private:

    DatabaseManager& databaseManager = DatabaseManager::getInstance();
    const std::string DATABASE_FLAG = "-d";
    const std::string BTREE_FLAG = "-b";

    std::string getDescription() override {
        return "Prints the bTree structure or the database";
    } 
    
    public:

    PrintCommand() {
        this->flags = {
            {Commands::HELP_FLAG, false},
            {DATABASE_FLAG, false},
            {BTREE_FLAG, false},

        };
    }

    void execute(std::vector<std::string> tokens) override
    {
        if(!this->setFlags(tokens)) return;

        if (this->flags[Commands::HELP_FLAG]) {
            this->printHelp();
            return;
        }

        if (this->flags[DATABASE_FLAG])
            databaseManager.readAllDataFromDatabase();
        
        else if (this->flags[BTREE_FLAG])
            databaseManager.readBTree();

        else
            std::cout << "Error, no flag provided" << std::endl;
    }
    
   
};