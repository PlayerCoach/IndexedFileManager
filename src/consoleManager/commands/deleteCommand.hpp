#include "ICommand.hpp"
#include "commands.hpp"
#include "databaseManager.hpp"

class DeleteCommand : public Command
{
    private:

    std::string getDescription() override {
        return "Deletes record with a given key from the database";
    }

    uint64_t key;
    DatabaseManager& dbManager = DatabaseManager::getInstance();

    public:

    DeleteCommand() {
        this->flags = {
            {Commands::HELP_FLAG, false},
        };
    }

    void execute(std::vector<std::string> tokens) override
    {
        this->trySetKey(tokens);

        if(!this->setFlags(tokens)) return;

        if (this->flags[Commands::HELP_FLAG]) {
            this->printHelp();
            return;
        }

        this->deleteKey();    
    }

    private:

    void trySetKey(std::vector<std::string>& tokens) {
      
        try 
        {
             this->key = std::stoull(tokens[0]);
        }
        catch (std::invalid_argument& e) 
        {
            return;
        }
        tokens.erase(tokens.begin());
    }

    void deleteKey() 
    {
        std::string result = dbManager.deleteRecordFromDatabase(key);
        std::cout << result << std::endl;
    }
    
};