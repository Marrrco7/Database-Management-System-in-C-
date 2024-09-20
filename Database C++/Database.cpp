#include "Database.h"
#include "fmt/core.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>

//helper method to determine if a specific row in the database meets the criteria that is specified in the condition. Trying to keep select method cleaner.
bool evaluateCondition(const std::vector<std::string>& row, const std::vector<Column>& columns, const std::string& condition) {
    std::istringstream iStringStream(condition);
    std::string column, op, value;
    iStringStream >> column >> op >> std::ws;
    std::getline(iStringStream, value);
    value.erase(std::remove(value.begin(), value.end(), '\''), value.end());

    // Find the column index
    auto it = std::find_if(columns.begin(), columns.end(), [&column](const Column& col) { return col.name == column; });
    if (it == columns.end()) {
        return false;
    }
    size_t colIndex = std::distance(columns.begin(), it);

    // operator comparison
    if (op == "=") {
        return row[colIndex] == value;
    } else if (op == "!=") {
        return row[colIndex] != value;
    } else if (op == "<") {
        return row[colIndex] < value;
    } else if (op == "<=") {
        return row[colIndex] <= value;
    } else if (op == ">") {
        return row[colIndex] > value;
    } else if (op == ">=") {
        return row[colIndex] >= value;
    }

    return false;
}

// Helper method to check if Integer representation
bool isInteger(const std::string& str) {
    if (str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+'))) return false;
    char* p;
    strtol(str.c_str(), &p, 10);
    return (*p == 0);
}

// Helper method to check if string representation
bool isFloat(const std::string& str) {
    char* endptr;
    strtod(str.c_str(), &endptr);
    return (*endptr == '\0');
}

// Helper method to check if date
bool isDate(const std::string& str) {
    if (str.size() != 10 || str[4] != '-' || str[7] != '-') return false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(str[i])) return false;
    }
    return true;
}

void Database::createTable(const std::string& tableName, const std::vector<Column>& columns) {
    Table newTable;
    newTable.name = tableName;
    newTable.columns = columns;
    tables[tableName] = newTable;

    fmt::print("Table '{}' created successfully.\n", tableName);
}


void Database::deleteTable(const std::string& tableName) {
    if (tables.find(tableName) != tables.end()) {
        tables.erase(tableName);
        fmt::print("Table '{}' deleted successfully.\n", tableName);
    } else {
        fmt::print(stderr, "Table '{}' not found.\n", tableName);
    }
}

/*bool Database::checkPrimaryKey(const Table& table, const std::vector<std::string>& rowData) {
    for (const Column& column : table.columns) {
        if (column.isPrimaryKey) {
            size_t colIndex = &column - &table.columns[0];
            for (const auto& row : table.rows) {
                if (row[colIndex] == rowData[colIndex]) {
                    return false;
                }
            }
        }
    }
    return true;
}*/


void Database::insertRow(const std::string& tableName, const std::vector<std::string>& rowData) {
    if (tables.find(tableName) != tables.end()) {
        Table& table = tables[tableName];
        if (rowData.size() != table.columns.size()) {
            fmt::print(stderr, "Error: Column count does not match, therefore cannot insert row.\n");
            return;
        }

        //check the datatypes
        for (size_t i = 0; i < rowData.size(); ++i) {
            const Column& column = table.columns[i];
            const std::string& data = rowData[i];
            if (column.type == DataTypes::INT && !isInteger(data)) {
                fmt::print(stderr, "Error: Data type mismatch for column '{}'. Expected a type: INT.\n", column.name);
                return;
            } else if (column.type == DataTypes::FLOAT && !isFloat(data)) {
                fmt::print(stderr, "Error: Data type mismatch for column '{}'. Expected a type: FLOAT.\n", column.name);
                return;
            } else if (column.type == DataTypes::DOUBLE && !isFloat(data)) {
                fmt::print(stderr, "Error: Data type mismatch for column '{}'. Expected a type: DOUBLE.\n", column.name);
                return;
            } else if (column.type == DataTypes::DATE && !isDate(data)) {
                fmt::print(stderr, "Error: Data type mismatch for column '{}'. Expected format: (YYYY-MM-DD).\n", column.name);
                return;
            }
        }

        table.rows.push_back(rowData);
        fmt::print("Row inserted into table '{}'.\n", tableName);
    } else {
        fmt::print(stderr, "Error: Table '{}' not found. Cannot insert row.\n", tableName);
    }
}


void Database::select(const std::string& tableName, const std::string& condition) {
    if (tables.find(tableName) != tables.end()) {
        const Table& table = tables[tableName];

        std::vector<size_t> columnWidths(table.columns.size(), 0);
        for (size_t i = 0; i < table.columns.size(); ++i) {
            columnWidths[i] = table.columns[i].name.length();
        }
        for (const auto& row : table.rows) {
            for (size_t i = 0; i < row.size(); ++i) {
                if (row[i].length() > columnWidths[i]) {
                    columnWidths[i] = row[i].length();
                }
            }
        }

        fmt::print("Contents of table '{}':\n", tableName);

        for (size_t i = 0; i < table.columns.size(); ++i) {
            fmt::print("{:<{}}  ", table.columns[i].name, columnWidths[i]);
        }
        fmt::print("\n");

        for (const auto& row : table.rows) {
            if (condition.empty() || evaluateCondition(row, table.columns, condition)) {
                for (size_t i = 0; i < row.size(); ++i) {
                    fmt::print("{:<{}}  ", row[i], columnWidths[i]);
                }
                fmt::print("\n");
            }
        }
    } else {
        fmt::print("Table '{}' does not exist.\n", tableName);
    }
}

void Database::addColumn(const std::string& tableName, const std::string& columnName, DataTypes columnType) {
    if (tables.find(tableName) != tables.end()) {
        Table& table = tables[tableName];
        table.columns.push_back({columnName, columnType});
        for (auto& row : table.rows) {
            row.push_back(""); // this is to initialize new column with empty strings
        }
        fmt::print("Column '{}' added to table '{}'.\n", columnName, tableName);
    } else {
        fmt::print(stderr, "Error: Table '{}' not found. Cannot add column.\n", tableName);
    }
}

void Database::userQuery(const std::string& query) {
    std::istringstream iss(query);
    std::string command;
    iss >> command;
    command.erase(std::remove(command.end() - 1, command.end(), ';'), command.end());
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "CREATE") {
        std::string table, tableName;
        iss >> table >> tableName;
        tableName.erase(std::remove(tableName.end() - 1, tableName.end(), ';'), tableName.end());
        if (table == "TABLE") {
            std::string columnsStr;
            std::getline(iss, columnsStr, '(');
            std::getline(iss, columnsStr, ')');
            std::istringstream columnsStream(columnsStr);
            std::vector<Column> columns;
            std::string column;
            while (std::getline(columnsStream, column, ',')) {
                std::istringstream colStream(column);
                std::string colName, colType;
                bool isPrimaryKey = false;
                colStream >> colName >> colType;
                std::string word;
                while (colStream >> word) {
                    if (word == "PRIMARY" || word == "KEY") {
                        isPrimaryKey = true;
                    } else if (colType.empty()) {
                        colType = word;
                    }
                }

                DataTypes type;
                if (colType == "INT") {
                    type = DataTypes::INT;
                } else if (colType == "STRING") {
                    type = DataTypes::STRING;
                } else if (colType == "FLOAT") {
                    type = DataTypes::FLOAT;
                } else if (colType == "DOUBLE") {
                    type = DataTypes::DOUBLE;
                } else if (colType == "DATE") {
                    type = DataTypes::DATE;
                } else {
                    fmt::print(stderr, "Error: Unsupported data type '{}'.\n", colType);
                    return;
                }

                columns.push_back({colName, type});
            }
            createTable(tableName, columns);
        }
    } else if (command == "INSERT") {
        std::string into, tableName, valuesStr;
        iss >> into >> tableName >> valuesStr;
        tableName.erase(std::remove(tableName.end() - 1, tableName.end(), ';'), tableName.end());
        if (into == "INTO" && valuesStr == "VALUES") {
            std::string rowStr;
            std::getline(iss, rowStr, '(');
            std::getline(iss, rowStr, ')');
            std::istringstream rowStream(rowStr);
            std::vector<std::string> rowData;
            std::string value;
            while (std::getline(rowStream, value, ',')) {
                value.erase(std::remove(value.begin(), value.end(), '\''), value.end()); // Remove single quotes from value
                value.erase(0, value.find_first_not_of(" \t")); // Remove leading whitespace
                value.erase(value.find_last_not_of(" \t") + 1); // Remove trailing whitespace
                rowData.push_back(value);
            }
            insertRow(tableName, rowData);
        }
    } else if (command == "SELECT") {
        std::string star, from, tableName;
        iss >> star >> from >> tableName;
        tableName.erase(std::remove(tableName.end() - 1, tableName.end(), ';'), tableName.end());
        if (star == "*" && from == "FROM") {
            std::string whereClause;
            std::getline(iss, whereClause);
            whereClause.erase(std::remove(whereClause.begin(), whereClause.end(), ';'), whereClause.end());
            if (!whereClause.empty()) {
                std::istringstream whereStream(whereClause);
                std::string where;
                whereStream >> where;
                if (where == "WHERE") {
                    std::string condition;
                    std::getline(whereStream, condition);
                    condition.erase(0, condition.find_first_not_of(" \t"));
                    select(tableName, condition);
                } else {
                    fmt::print(stderr, "Error: Invalid SELECT query syntax.\n");
                }
            } else {
                select(tableName);
            }
        } else {
            fmt::print(stderr, "Error: Invalid SELECT query syntax.\n");
        }
    } else if (command == "DELETE") {
        std::string from, tableName;
        iss >> from >> tableName;
        tableName.erase(std::remove(tableName.end() - 1, tableName.end(), ';'), tableName.end());
        if (from == "FROM") {
            deleteTable(tableName);
        }
    } else if (command == "ALTER") {
        std::string table, tableName, add, columnName, columnType;
        iss >> table >> tableName >> add >> columnName >> columnType;
        tableName.erase(std::remove(tableName.end() - 1, tableName.end(), ';'), tableName.end());
        if (table == "TABLE" && add == "ADD") {
            DataTypes type;
            if (columnType == "INT") {
                type = DataTypes::INT;
            } else if (columnType == "STRING") {
                type = DataTypes::STRING;
            } else if (columnType == "FLOAT") {
                type = DataTypes::FLOAT;
            } else if (columnType == "DOUBLE") {
                type = DataTypes::DOUBLE;
            } else if (columnType == "DATE") {
                type = DataTypes::DATE;
            } else {
                fmt::print(stderr, "Error: Unsupported data type '{}'.\n", columnType);
                return;
            }
            addColumn(tableName, columnName, type);
        }
    }else {
        fmt::print(stderr, "Invalid query command.\n");
    }
}


bool Database::saveToFile(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file) {
        return false;
    }

    for (const auto& tablePair : tables) {
        const Table& table = tablePair.second;

        file << "CREATE TABLE " << table.name << " (";
        for (size_t i = 0; i < table.columns.size(); ++i) {
            if (i > 0) file << ", ";
            file << table.columns[i].name << " " << (table.columns[i].type == DataTypes::INT ? "INT" :table.columns[i].type == DataTypes::STRING ? "STRING" : table.columns[i].type == DataTypes::FLOAT ? "FLOAT" : table.columns[i].type == DataTypes::DOUBLE ? "DOUBLE" : "DATE");
        }
        file << ");\n";

        for (const auto& row : table.rows) {
            file << "INSERT INTO " << table.name << " VALUES (";
            for (size_t i = 0; i < row.size(); ++i) {
                if (i > 0) file << ", ";
                file << "'" << row[i] << "'";
            }
            file << ");\n";
        }
    }

    return true;
}



bool Database::loadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        return false;
    }

    tables.clear();

    std::string line;
    while (std::getline(file, line)) {
        userQuery(line);
    }

    return true;
}



