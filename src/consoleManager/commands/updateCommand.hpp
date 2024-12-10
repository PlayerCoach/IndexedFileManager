#include "ICommand.hpp"
#include "commands.hpp"
#include <iostream>
#include "record.hpp"
#include "databaseManager.hpp"


class UpdateCommand : public Command 
{
    private:

    DatabaseManager& databaseManager = DatabaseManager::getInstance();

    std::string getDescription() override {
        return "Updates record in the database";
    } 
    

    public:

    UpdateCommand() {
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

        updateRecord();



    }

    void updateRecord()
    {
        std::cout << "Enter Key: ";
        std::string keyInput;
        std::getline(std::cin, keyInput);
    
        uint64_t key;
        try
        {
            key = std::stoull(keyInput); // Convert string to uint64_t
        }
        catch(const std::invalid_argument& e)
        {
            std::cerr << e.what() << '\n';
            return;
        }
        
        std::cout << "Enter record (space-separated integers): ";
        std::string recordInput;
        std::getline(std::cin, recordInput);
        std::istringstream recordStream(recordInput);
        std::vector<int32_t> record;
        int32_t number;
        while (recordStream >> number) 
        {
            record.push_back(number);
        }
        if(record.size() == 0)
        {
            std::cout << "Record is empty" << std::endl;
            return;
        }
        else
        {
            databaseManager.updateRecordInDatabase(key, Record(record));
        }
    }
    
    
};