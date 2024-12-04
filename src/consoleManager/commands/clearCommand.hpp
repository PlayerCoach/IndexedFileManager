#include "commands.hpp"
#include "ICommand.hpp"

class ClearCommand : public Command
{
    private:
    std::string getDescription() override {
        return "Clears the screen";
    }

    public:
    ClearCommand() {
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

        this->clearScreen();
    }

    void clearScreen()
    {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
};