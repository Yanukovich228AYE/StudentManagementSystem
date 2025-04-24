//
// Created by nikita on 4/23/25.
//

#ifndef TEACHER_H
#define TEACHER_H

#include <string>
#include <vector>

struct Teacher {
    // int id; // not needed because it's autoincrement in the database
    std::string name;
    float salary;

    std::vector<std::string> get_columns() const {
        return {"name", "salary"};
    }

    std::vector<std::string> get_values() const {
        return {name, std::to_string(salary)};
    }

    static std::vector<std::pair<std::string, std::string>> get_schema() {
        return {
                {"id", "INT PRIMARY KEY AUTO_INCREMENT"},
                {"name", "VARCHAR(100) NOT NULL"},
                {"salary", "float"}
        };
    }
};

#endif //TEACHER_H
