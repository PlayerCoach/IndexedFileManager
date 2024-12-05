#include <iostream>
#include "const.hpp"
#include "consoleManager.hpp"
#include "dataEntry.hpp"
#include "databaseManager.hpp"

int main() {

    //initialize database
    DatabaseManager& databaseManager = DatabaseManager::getInstance();

    ConsoleManager consoleManager;
    consoleManager.getUserInput();
    return 0;
}