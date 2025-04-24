#include "db.h"

bool insert_in_table(MYSQL *conn, const std::string &table_name, const std::vector<std::string> &columns, const std::vector<std::string> &values) {
    if (columns.size() != values.size()) {
        std::cout << "Columns and values count don't match" << std::endl;
    }

    std::string query = "INSERT INTO " + table_name + " (";

    for (size_t i = 0; i < columns.size(); i++) {
        query += columns[i];
        if (i != columns.size() - 1)
            query += ", ";
    }
    query += ") VALUES (";

    for (size_t i = 0; i < values.size(); i++) {
        bool is_number = !values[i].empty() &&
                                 std::find_if(values[i].begin(),
                                              values[i].end(),
                                              [](unsigned char c) { return !std::isdigit(c) && c != '.'; }) == values[i].end();

        if (!is_number)
            query += "'"+values[i]+"'";
        else
            query += values[i];
        if (i != values.size() - 1)
            query += ", ";
    }
    query += ")";

    if (mysql_query(conn, query.c_str())) {
        std::cout << "Insert failed: " << mysql_error(conn) << std::endl;
        return false;
    }

    return true;
}

void print(MYSQL *conn, const std::string &table_name) {
    std::string query = "SELECT * FROM " + table_name;
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "SELECT error: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "mysql_store_result failed: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_ROW row;
    while (row = mysql_fetch_row(result)) {
        if (!table_name.compare("students")) {
            std::cout << "<----- Student List ----->" << std::endl;
            std::cout << "ID: " << row[0]
                << ", Name: " << row[1]
                << ", GPA: " << row[2] << std::endl;
        } else if (!table_name.compare("teachers")) {
            std::cout << "<----- Teacher List ----->" << std::endl;
            std::cout << "ID: " << row[0]
                << ", Name: " << row[1]
                << ", Salary: " << row[2] << std::endl;
        }
    }
    std::cout << "<------------------------>" << std::endl;

    mysql_free_result(result);
}

bool create_table(MYSQL *conn, const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns) {
    // check if table exists
    std::string check_query = "SHOW TABLES LIKE '" + table_name + "'";
    if (mysql_query(conn, check_query.c_str())) {
        std::cerr << "Error checking table: " << mysql_error(conn) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "mysql_store_result failed: " << mysql_error(conn) << std::endl;
        return false;
    }

    bool exists = mysql_num_rows(result) > 0;
    mysql_free_result(result);

    if (exists)
        return true;

    // create table
    std::string query = "CREATE TABLE IF NOT EXISTS "+ table_name +" (";

    for (int i = 0; i < columns.size(); i++) {
        query += columns[i].first + " " + columns[i].second;
        if (i != columns.size()-1)
            query += ", ";
    }

    query += ")";

    if (mysql_query(conn, query.c_str())) {
        std::cout << "Table creation failed: " << mysql_error(conn) << std::endl;
        return false;
    }
    std::cout << "Table " << table_name << " has been successfully created" << std::endl;

    return true;
}

MYSQL *connect_to_db(const char *host, const char *user, const char *password, const char *database) {
    MYSQL* conn = mysql_init(NULL);
    if (!conn) {
        std::cerr << "mysql_init() failed" << std::endl;
        return NULL;
    }

    if (!mysql_real_connect(conn, host, user, password, database, 3306, NULL, 0)) {
        std::cerr << "mysql_real_connect() failed: " << mysql_error(conn) << std::endl;
        return NULL;
    }

    return conn;
}

bool drop_tables(MYSQL* conn) {
    std::string show_query = "SHOW TABLES;";

    if (mysql_query(conn, show_query.c_str())) {
        std::cout << "Show table failed: " << mysql_error(conn) << std::endl;
        return false;
    }

    MYSQL_RES *result = mysql_store_result();
}

void close_db(MYSQL *conn) {
    mysql_close(conn);
}