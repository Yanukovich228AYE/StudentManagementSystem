#include "db.h"

bool insert_entity(MYSQL *conn, const std::string &table_name, const User &entity) {
    std::string query = "INSERT INTO " + table_name + " (";

    std::vector<std::string> columns = entity.get_columns();
    for (size_t i = 0; i < columns.size(); i++) {
        query += columns[i];
        if (i != columns.size() - 1)
            query += ", ";
    }
    query += ") VALUES (";

    std::vector<std::string> values = entity.get_values();
    for (size_t i = 0; i < values.size(); i++) {
        bool is_number = !values[i].empty() &&
                         std::find_if(values[i].begin(),
                                      values[i].end(),
                                      [](unsigned char c) { return !std::isdigit(c) && c != '.'; }) == values[i].end();
        if (columns[i] == "password")
            query += "'" + hash(values[i]) + "'";
        else if (!is_number)
            query += "'" + values[i] + "'";
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

bool login(MYSQL *conn) {
    std::cout << "---------- LOGIN ----------" << std::endl;
    std::string username, password;
    std::cout << "username: ";
    std::cin >> username;
    std::cout << "password: ";
    std::cin >> password;

    std::vector<std::string> res = search_db(conn, {{"name", username}, {"password", hash(password)}}, {"students", "teachers"});
    for (const auto& row : res) {
        std::cout << row << " ";
    }
    std::cout << std::endl;
    return true;
}

std::string hash(std::string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *) str.c_str(), str.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int) hash[i];

    return ss.str();
}

void main_screen() {
    std::cout << "Welcome to school database!" << std::endl;
}

void print(MYSQL *conn, const std::string &table_name, std::vector<std::pair<std::string, std::string> > schema) {
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


    std::cout << "<----- " << table_name << " list ----->" << std::endl;
    while (row = mysql_fetch_row(result)) {
        for (int i = 0; i < schema.size(); i++)
            std::cout << schema[i].first << ": " << row[i] << " ";
        std::cout << std::endl;
    }

    std::cout << "<------------------------>" << std::endl;

    mysql_free_result(result);
}

bool create_table(soci::session& db, const std::string& dbname, const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns) {
    try {
        if (columns.empty()) {
            std::cerr << "No columns provided for table: " << table_name << std::endl;
            return false;
        }

        std::string query = "CREATE TABLE IF NOT EXISTS `" + dbname + "`.`" + table_name + "`";

        for (int i = 0; i < columns.size(); i++) {
            query += "`" + columns[i].first + "` " + columns[i].second;
            if (i < columns.size() - 1)
                query += ", ";
        }
        query += ")";

        db << query;
        std::cout << "Created table " << table_name << std::endl;
        return true;
    } catch (const soci::soci_error& e) {
        std::cerr << "SOCI error creating table `" << table_name << "`: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard error creating table `" << table_name << "`: " << e.what() << std::endl;
    }
    return false;
}

std::unique_ptr<soci::session> connect_to_db(const std::string& host, const std::string& user, const std::string& password, const std::string& database) {
    try {
        // port is 3306 since it's in local. adjust to your parameters if needed
        auto sql = std::make_unique<soci::session> (
            soci::mysql,
            "db="+database
            + " user="+user
            + " password="+password
            + " host="+host
            + " port=3306");

        return sql;
    } catch (const soci::soci_error& e) {
        std::cerr << "SOCI error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard error: " << e.what() << std::endl;
    }
    return nullptr;
}

bool clear_all_tables(soci::session& db, const std::string dbname) {
    try {
        soci::row row;
        soci::statement st = (db.prepare <<
            "SELECT table_name FROM information_schema.tables "
            "WHERE table_schema  = :db", soci::use(dbname), soci::into(row)
            );

        st.execute();
        while (st.fetch()) {
            std::string table = row.get<std::string>(0);
            try {
                db << "DELETE FROM `" + table + "`";
                std::cout << "Cleared table: " << table << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Failed to clear table `" << table << "`: " << e.what() << std::endl;
                return false;
            }
        }
        return true;
    } catch (const soci::soci_error& e) {
        std::cerr << "SOCI error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard error: " << e.what() << std::endl;
    }
    return false;
}

bool drop_tables(soci::session db, const std::string dbname) {
    try {
        soci::row row;
        soci::statement st = (db.prepare <<
            "SELECT table_name FROM information_schema.tables "
            "WHERE table_schema  = :db", soci::use(dbname), soci::into(row)
            );

        st.execute();
        while (st.fetch()) {
            std::string table = row.get<std::string>(0);
            try {
                db << "DROP TABLE `" + table + "`";
                std::cout << "Dropped table: " << table << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Failed to clear table `" << table << "`: " << e.what() << std::endl;
                return false;
            }
        }
        return true;
    } catch (const soci::soci_error& e) {
        std::cerr << "SOCI error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard error: " << e.what() << std::endl;
    }
    return false;
}