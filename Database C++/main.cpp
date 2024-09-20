#include <iostream>
#include <sstream>
#include "Database.h"
#include "fmt/core.h"

int main() {
    Database myDatabase;
    std::string query;

    fmt::print("Enter SQL queries (type 'load file_path' to load ----  'exit' to quit ---- 'save file_path' to save):\n");
    while (true) {
        fmt::print("~> ");
        std::getline(std::cin, query);
        if (query == "EXIT") {
            break;
        } else if (query.find("SAVE ") == 0) {
            std::string filePath = query.substr(5);
            if (myDatabase.saveToFile(filePath)) {
                fmt::print("Database saved to '{}'\n", filePath);
            } else {
                fmt::print(stderr, "Error: Could not be saved to file '{}'\n", filePath);
            }
        } else if (query.find("LOAD ") == 0) {
            std::string filePath = query.substr(5);
            if (myDatabase.loadFile(filePath)) {
                fmt::print("Database loaded from '{}'\n", filePath);
            } else {
                fmt::print(stderr, "Error: Could not be loaded from the file. '{}'\n", filePath);
            }
        } else {
            myDatabase.userQuery(query);
        }
    }

    return 0;
}
