#include "ICommand.hpp"
#include "commands.hpp"
#include <iostream>
#include <unordered_map>
#include "dataEntry.hpp"
#include <memory>
#include "databaseManager.hpp"


class AddCommand : public Command {

    private :
    const std::string RANDOM_FLAG = "-r";
    const std::string USER_FLAG = "-u";
    DatabaseManager& databaseManager = DatabaseManager::getInstance();

    int count = 0;

    public:

    AddCommand() {
        this->flags = {
            {Commands::HELP_FLAG, false},
            {RANDOM_FLAG, false},
            {USER_FLAG, false},
        };
    }

    void execute(std::vector<std::string> tokens) override
    {
        this->setCount(tokens);
        
        if(!this->setFlags(tokens)) return;

        if (this->flags[Commands::HELP_FLAG]) {
            this->printHelp();
            return;
        }

        if (this->checkFlagsConflict()) return;


        if (this->flags[RANDOM_FLAG])
            addRandomData(count);
        
        else if (this->flags[USER_FLAG])
            addUserData(count);

        else
            std::cout << "Error, no flag provided" << std::endl;
    }

    private:

    std::string getDescription() override 
    {
        return "Adds [int] number to file"
        "either random ["+ RANDOM_FLAG +"] or user input ["+ USER_FLAG +"]";
    } 

    bool checkFlagsConflict() override
    {
        if (this->flags[RANDOM_FLAG] && this->flags[USER_FLAG])
        {
            std::cout << "Flags conflict. Cannot use both flags" << std::endl;
            return true;
        }
        return false;
    }

    void setCount(std::vector<std::string>& tokens)
    {
        try
        {
            count = stoi(tokens[0]);
            
        }
        catch(const std::invalid_argument& e)
        {
            return;
        }
        tokens.erase(tokens.begin());
        return;
    }

    void addRandomData(int count)
    {
       static uint64_t key = 0;
       Record record;
       DataEntry dataEntry(record, key);
       key++;
       std::cout  << dataEntry << std::endl;
       databaseManager.writeDataToDatabase(dataEntry);
       //databaseManager.readDataFromDatabase(0);
       
    }

    void addUserData(int count)
{
    std::cout << "Enter Key: ";
    std::string keyInput;
    std::getline(std::cin, keyInput);
    uint64_t key = std::stoull(keyInput); // Convert string to uint64_t

    std::cout << "Enter record (space-separated integers): ";
    std::string recordInput;
    std::getline(std::cin, recordInput);
    std::istringstream recordStream(recordInput);
    std::vector<int32_t> record;
    int32_t number;
    while (recordStream >> number) {
        record.push_back(number);
    }

    DataEntry dataEntry(Record(record), key);
    databaseManager.writeDataToDatabase(dataEntry);
}

};