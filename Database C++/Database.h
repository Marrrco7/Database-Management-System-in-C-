//
// Created by marco on 5/6/2024.
//


#include <string>
#include <vector>
#include <map>
#ifndef DATABASE_H
#define DATABASE_H

enum class DataTypes {
    INT,
    FLOAT,
    DOUBLE,
    STRING,
    DATE
};


struct Column {
    std::string name;
    DataTypes type;

    bool isPrimaryKey = false;
    std::string foreignKeyTable;
    std::string foreignKeyColumn;
};

struct Table {
    std::string name;
    std::vector<Column> columns;
    std::vector<std::vector<std::string>> rows;
};


class Database {
private:
    std::map<std::string, Table> tables;

public:
    void createTable(const std::string& tableName, const std::vector<Column>& columns);
    void deleteTable(const std::string& tableName);
    void insertRow(const std::string& tableName, const std::vector<std::string>& rowData);
    void select(const std::string& tableName, const std::string& condition = "");
    void userQuery(const std::string& query);
    void addColumn(const std::string& tableName, const std::string& columnName, DataTypes columnType);
    /*bool checkPrimaryKey(const Table& table, const std::vector<std::string>& rowData);
    bool checkForeignKey(const Table& table, const std::vector<std::string>& rowData);*/
    bool saveToFile(const std::string& filePath);
    bool loadFile(const std::string& filePath);
};
#endif








