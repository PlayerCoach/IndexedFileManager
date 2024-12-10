#include "ICommand.hpp"
#include "commands.hpp"

class SearchCommand : public Command{
    public:
    void execute(const std::vector<std::string>& tokens) override;
};
