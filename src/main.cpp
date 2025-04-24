#include "db.h"
#include "student.h"
#include "teacher.h"

int main() {
    MYSQL* conn = connect_to_db("localhost", "root", "root", "test");
    Student *student = new Student;
    Teacher *teacher = new Teacher;

    if (conn) {
        std::cout << "Successfully connected to the database!" << std::endl;

        std::string table_name = "students";
        std::vector<std::pair<std::string, std::string>> student_param = student->get_schema();

        if (create_table(conn, table_name, student_param)) {
            std::vector<std::string> columns = student->get_columns();
            std::vector<std::string> values = {"Nikita", "3.5"};
            if (insert_in_table(conn, table_name, columns, values)) {
                std::cout << values[0] << " successfully inserted in " << table_name << " table" << std::endl;
            }

            print(conn, table_name);
        }

        table_name = "teachers";
        std::vector<std::pair<std::string, std::string>> teacher_param = teacher->get_schema();

        if (create_table(conn, table_name, teacher_param)) {
            std::vector<std::string> columns = teacher->get_columns();
            std::vector<std::string> values = {"Cottignoli", "35000"};
            if (insert_in_table(conn, table_name, columns, values)) {
                std::cout << values[0] << " successfully inserted in " << table_name << " table" << std::endl;
            }

            print(conn, table_name);
        }

        close_db(conn);
    } else {
        std::cerr << "Failed to connect to db" << std::endl;
    }


    return 0;
}