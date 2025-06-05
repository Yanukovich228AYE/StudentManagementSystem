#include "libs.h"
#include "db.h"
#include "User.h"

std::optional<User> search_db(soci::session& db, const std::string& table_name, const std::string& key, const std::string& value) {
    try {
        // Validate column using User::get_columns()
        User temp_user;
        const auto& valid_columns = temp_user.get_insertable_columns();
        if (std::find(valid_columns.begin(), valid_columns.end(), key) == valid_columns.end()) {
            throw std::invalid_argument("Invalid search key: " + key);
        }

        if (table_name != "users") {
            throw std::invalid_argument("Invalid table name");
        }

        int uid;
        std::string uname, upass;
        double ugpa_value;
        soci::indicator ugpa_ind;
        std::string usubject_value;
        soci::indicator usubject_ind;
        int urole_int;

        // Construct query with validated key
        std::string query = "SELECT id, name, password, gpa, subject, role FROM " + table_name + " WHERE " + key + " = :value";

        db << query,
            soci::into(uid),
            soci::into(uname),
            soci::into(upass),
            soci::into(ugpa_value, ugpa_ind),
            soci::into(usubject_value, usubject_ind),
            soci::into(urole_int),
            soci::use(value);

        std::optional<double> ugpa;
        if (ugpa_ind == soci::i_ok)
            ugpa = ugpa_value;

        std::optional<std::string> usubject;
        if (usubject_ind == soci::i_ok)
            usubject = usubject_value;

        UserRole role = static_cast<UserRole>(urole_int);
        return User(uid, uname, upass, ugpa, usubject, role);

    } catch (const soci::soci_error& e) {
        std::cerr << "SOCI error while searching in table `" << table_name << "`: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard error while searching in table `" << table_name << "`: " << e.what() << std::endl;
    }

    return std::nullopt;
}


bool insert_user(soci::session& db, const std::string& table_name, const User& user) {
    try {
        // Get columns and values without 'id' since it's auto-incremented
        auto columns = user.get_insertable_columns();
        auto values = user.get_insertable_values();

        std::string query = "INSERT INTO " + table_name + " (";
        for (size_t i = 0; i < columns.size(); ++i) {
            query += columns[i];
            if (i < columns.size() - 1) query += ", ";
        }
        query += ") VALUES (";
        for (size_t i = 0; i < columns.size(); ++i) {
            query += ":" + columns[i];
            if (i < columns.size() - 1) query += ", ";
        }
        query += ")";

        soci::statement st = (db.prepare << query);

        // Bind parameters in the correct order
        // Note: The order must match the columns in get_insertable_columns()
        st.exchange(soci::use(values[0], "name"));
        st.exchange(soci::use(values[1], "password"));

        // Handle optional gpa
        soci::indicator gpa_ind = user.get_gpa().has_value() ? soci::i_ok : soci::i_null;
        double gpa_value = user.get_gpa().value_or(0.0);
        st.exchange(soci::use(gpa_value, gpa_ind, "gpa"));

        // Handle optional subject
        soci::indicator subject_ind = user.get_subject().has_value() ? soci::i_ok : soci::i_null;
        std::string subject_value = user.get_subject().value_or("");
        st.exchange(soci::use(subject_value, subject_ind, "subject"));

        // Role is always required
        int role_value = static_cast<int>(user.get_role());
        st.exchange(soci::use(role_value, "role"));

        st.define_and_bind();
        st.execute(true);  // Execute the statement

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error inserting user: " << e.what() << std::endl;
        return false;
    }
}


bool create_table(soci::session& db, const std::string& dbname, const std::string& table_name,
                 const std::vector<std::pair<std::string, std::string>>& columns) {
    try {
        if (columns.empty()) {
            std::cerr << "No columns provided for table: " << table_name << std::endl;
            return false;
        }

        std::string query = "CREATE TABLE IF NOT EXISTS `" + dbname + "`.`" + table_name + "` (";

        for (size_t i = 0; i < columns.size(); ++i) {
            query += columns[i].first + " " + columns[i].second;
            if (i < columns.size() - 1) {
                query += ", ";
            }
        }

        query += ")";  // Single closing parenthesis

        std::cout << "Executing query: " << query << std::endl;  // Debug output
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
        // port is 3306 for me. adjust to your parameters if needed
        auto sql = std::make_unique<soci::session> (
            soci::mysql,
            "db="+database
            + " user="+user
            + " password="+password
            + " host="+host
            + " port=3306"
            );

        return sql;
    } catch (const soci::soci_error& e) {
        std::cerr << "SOCI error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard error: " << e.what() << std::endl;
    }
    return nullptr;
}

bool clear_all_tables(soci::session& db, const std::string& dbname) {
    try {
        soci::row row;
        soci::statement st = (db.prepare <<
            "SELECT table_name FROM information_schema.tables "
            "WHERE table_schema  = :db", soci::use(dbname), soci::into(row)
            );

        st.execute();
        while (st.fetch()) {
            auto table = row.get<std::string>(0);
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

bool drop_tables(soci::session& db, const std::string& dbname) {
    try {
        soci::row row;
        soci::statement st = (db.prepare <<
            "SELECT table_name FROM information_schema.tables "
            "WHERE table_schema  = :db", soci::use(dbname), soci::into(row)
            );

        st.execute();
        while (st.fetch()) {
            auto table = row.get<std::string>(0);
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

std::string hash(const std::string& str) {
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    SHA256(reinterpret_cast<const unsigned char*>(str.c_str()), str.size(), hash.data());

    std::stringstream ss;
    for (unsigned char byte : hash)
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);

    return ss.str();
}

void print_table(soci::session& db, const std::string& table_name) {
    try {
        // Validate known table
        if (table_name != "users") {
            throw std::invalid_argument("Unsupported table: " + table_name);
        }

        soci::rowset<soci::row> rs = (db.prepare << "SELECT * FROM " + table_name);

        std::cout << "Table: " << table_name << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        for (const auto& row : rs) {
            for (std::size_t i = 0; i != row.size(); ++i) {
                const soci::column_properties& props = row.get_properties(i);
                std::cout << props.get_name() << ": ";

                if (row.get_indicator(i) == soci::i_null) {
                    std::cout << "NULL";
                } else {
                    switch (props.get_data_type()) {
                        case soci::dt_string:
                            std::cout << row.get<std::string>(i);
                            break;
                        case soci::dt_double:
                            std::cout << row.get<double>(i);
                            break;
                        case soci::dt_integer:
                            std::cout << row.get<int>(i);
                            break;
                        case soci::dt_long_long:
                            std::cout << row.get<long long>(i);
                            break;
                        case soci::dt_unsigned_long_long:
                            std::cout << row.get<unsigned long long>(i);
                            break;
                        default:
                            std::cout << "[Unsupported Type]";
                    }
                }

                std::cout << "\t";
            }
            std::cout << std::endl;
        }

        std::cout << "----------------------------------------" << std::endl;

    } catch (const soci::soci_error& e) {
        std::cerr << "SOCI error while printing table `" << table_name << "`: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard error while printing table `" << table_name << "`: " << e.what() << std::endl;
    }
}