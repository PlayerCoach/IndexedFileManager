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
    bool shortFormInput = false;

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
        if(shortFormInput)
        {
            return;
        }
        
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

        if(tokens.size() == 0)
        {
            std::string result;
            uint64_t key = this->count;
            Record record("random");
            DataEntry dataEntry(record, key);
            result = databaseManager.writeDataToDatabase(dataEntry);
            if(result != "")
            {
                std::cout << result << std::endl;
            }
            shortFormInput = true;
            return;
        }
        return;
    }

    void addRandomData(int count)
    {
       for(int i = 0; i < count; i++)
       {
              DataEntry dataEntry;
              std::string result;
              result = databaseManager.writeDataToDatabase(dataEntry);
              if(result != "")
              {
                  std::cout << result << std::endl;
              }
       }
       
    }

    void addUserData(int count)
{
    for(int i = 0; i < count; i++)
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
            Record record("random");
        }
        else
        {
            Record record(record);
        }

        DataEntry dataEntry(record, key);
        std::string result;
        result = databaseManager.writeDataToDatabase(dataEntry);
        if(result != "")
        {
            std::cout << result << std::endl;
        }
}
}

};