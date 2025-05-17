#include "db.h"
#include "student.h"
#include "teacher.h"

int main() {
    try {
        std::unique_ptr<soci::session> db = connect_to_db("localhost", "root", "root", "test");
        if (!db)
            return 1;
        std::cout << "successfully connected to the database" << std::endl;

        drop_tables(*db, "test");

        create_table();

    } // database connection finishes here
    catch (const std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
    }
    return 0;
}
