#ifndef DB_H
#define DB_H

#include <iostream>
#include <mysql/mysql.h> // for mysql functions
#include <openssl/sha.h> // for hashing
#include <algorithm>
#include <iomanip>
#include <string>
#include <cstring>
#include <vector>
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>
#include "Entity.h"

bool login(MYSQL *conn);
bool registration();
void main_screen();

std::unique_ptr<soci::session> connect_to_db(const std::string& host, const std::string& user, const std::string& password, const std::string& database);
bool clear_all_tables(soci::session& db, const std::string dbname);
bool drop_tables(soci::session& db, const std::string dbname);

// std::vector<> -> dynamic array for new table parameters
// std::pair<> -> contains parameters, 1-st = parameters value, 2-nd = parameter type

bool insert_entity();

bool create_table(soci::session& db, const std::string& dbname, const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns);
std::vector<std::string> search_db(MYSQL *conn, const std::vector<std::pair<std::string, std::string>> &parameters, const std::vector<std::string> &tables_to_search);

void print(MYSQL* conn, const std::string &table_name, std::vector<std::pair<std::string, std::string>> schema);
std::string hash(std::string str);

// query is std::string (c++), mysql_query needs c-type parameter which is const char* so query.c_str() (std::string) -> const char*
// vector.size() returns size_t so in for loop better use size_t instead of int to prevent signed/unsigned conflicts
// mysql_query return 0 if succeded, and non-zero if failed

#endif //DB_H
