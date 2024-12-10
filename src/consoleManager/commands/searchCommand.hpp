#include "ICommand.hpp"
#include "commands.hpp"
#include "databaseManager.hpp"


class SearchCommand : public Command{
   private:

    std::string getDescription() override {
        return "Searches for and prints data entry with given key";
    }

    uint64_t key;
    DatabaseManager& dbManager = DatabaseManager::getInstance();

    public:

    SearchCommand() {
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
        std::string result = dbManager.searchForDataInDatabase(key);
        if(result.empty())
        {
            return; // operation was successful
        }
        std::cout << result << std::endl;
    }
};
