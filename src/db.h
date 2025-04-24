//
// Created by nikita on 4/22/25.
//

#ifndef DB_H
#define DB_H

#include <iostream>
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <algorithm>

MYSQL* connect_to_db(const char* host, const char* user, const char* password, const char* database);
bool drop_tables(MYSQL* conn);
void close_db(MYSQL* conn);

// std::vector<> -> dynamic array for new table parameters
// std::pair<> -> contains parameters, 1-st = parameters value, 2-nd = parameter type
bool create_table(MYSQL *conn, const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns);
bool insert_in_table(MYSQL *conn, const std::string &table_name, const std::vector<std::string> &columns, const std::vector<std::string> &values);

void print(MYSQL* conn, const std::string &table_name);

// query is std::string (c++), mysql_query needs c-type parameter which is const char* so query.c_str() (std::string) -> const char*
// vector.size() returns size_t so in for loop better use size_t instead of int to prevent signed/unsigned conflicts
// mysql_query return 0 if succeded, and non-zero if failed

#endif //DB_H
