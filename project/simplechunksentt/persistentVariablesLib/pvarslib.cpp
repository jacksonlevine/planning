#include <iostream>
#include <sqlite3.h>
#include "pvarslib.hpp"

std::string PVarsContext::tableName;

Result::Result() : type(PVARSERROR), _thing(std::nullopt) {
    
}

std::string Result::value() {
    if (_thing.has_value())
    {
        return _thing.value();
    }
    else {
        return std::string("noexist");
    }
}

void Result::setValue(std::string val)
{
    _thing.emplace(val);
}

Result getDbVariable(const char* key)
{
    Result result;

    // Open a connection to the database file
    sqlite3* db;
    int rc = sqlite3_open("data.db", &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return result;
    }

    std::string selectValueQuery = "SELECT value FROM " + PVarsContext::tableName + " WHERE name='" + std::string(key) + "';";

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, selectValueQuery.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            const unsigned char* value = sqlite3_column_text(stmt, 0);


            result.type = PVARSRESULT;
            result.setValue(std::string(reinterpret_cast<const char*>(value)));

        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}

bool setDbVariable(const char* variable1, const char* value1) {
    // Open a connection to the database file
    sqlite3* db;
    int rc = sqlite3_open("data.db", &db);


    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Create a table to store variables
    std::string createTableQuery = "CREATE TABLE IF NOT EXISTS " + PVarsContext::tableName + " (name TEXT PRIMARY KEY, value TEXT);";
    rc = sqlite3_exec(db, createTableQuery.c_str(), 0, 0, 0);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot create table: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // Store variables in the database
    std::string insertQuery = "INSERT OR REPLACE INTO " + PVarsContext::tableName + " (name, value) VALUES (?, ?);";

    // Prepare the insert statement
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // Bind variables and execute the statement
    sqlite3_bind_text(stmt, 1, variable1, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value1, -1, SQLITE_STATIC);
    sqlite3_step(stmt);

    // Finalize the statement and close the database
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return true;
}