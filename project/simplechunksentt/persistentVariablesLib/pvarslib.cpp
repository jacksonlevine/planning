#include <iostream>
#include <sqlite3.h>

int main() {
    // Open a connection to the database file
    sqlite3* db;
    int rc = sqlite3_open("data.db", &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    // Create a table to store variables
    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS variables (name TEXT PRIMARY KEY, value TEXT);";
    rc = sqlite3_exec(db, createTableQuery, 0, 0, 0);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot create table: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return rc;
    }

    // Store variables in the database
    const char* insertQuery = "INSERT OR REPLACE INTO variables (name, value) VALUES (?, ?);";

    // Example variables
    const char* variable1 = "foo";
    const char* value1 = "bar";
    const char* variable2 = "hello";
    const char* value2 = "world";

    // Prepare the insert statement
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, insertQuery, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return rc;
    }

    // Bind variables and execute the statement
    sqlite3_bind_text(stmt, 1, variable1, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value1, -1, SQLITE_STATIC);
    sqlite3_step(stmt);

    sqlite3_bind_text(stmt, 1, variable2, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value2, -1, SQLITE_STATIC);
    sqlite3_step(stmt);

    // Finalize the statement and close the database
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return 0;
}