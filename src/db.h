#ifndef DB_H
#define DB_H

#include <iostream>
#include "libs.h"
#include "User.h"

bool login();
bool registration();
void main_screen();

std::unique_ptr<soci::session> connect_to_db(const std::string& host, const std::string& user, const std::string& password, const std::string& database);
std::optional<User> search_db(soci::session& db, const std::string& table_name, const std::string& key, const std::string& value);

bool insert_user(soci::session& db, const std::string& table_name, const User& user);

bool create_table(soci::session& db, const std::string& dbname, const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns);
bool clear_all_tables(soci::session& db, const std::string& dbname);
bool drop_tables(soci::session& db, const std::string& dbname);

void print_table(soci::session& db, const std::string& table_name);
std::string hash(const std::string& str);

// query is std::string (c++), mysql_query needs c-type parameter which is const char* so query.c_str() (std::string) -> const char*
// vector.size() returns size_t so in for loop better use size_t instead of int to prevent signed/unsigned conflicts
// mysql_query return 0 if succeded, and non-zero if failed

#endif //DB_H
