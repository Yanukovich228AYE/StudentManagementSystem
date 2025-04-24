//
// Created by nikita on 4/22/25.
//

#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

struct Student {
    // int id; // not needed because it's autoincrement in the database
    std::string name;
    float gpa;

    std::vector<std::string> get_columns() const {
        return {"name", "gpa"};
    }

    std::vector<std::string> get_values() const {
        return {name, std::to_string(gpa)};
    }

    static std::vector<std::pair<std::string, std::string>> get_schema() {
        return {
            {"id", "INT PRIMARY KEY AUTO_INCREMENT"},
            {"name", "VARCHAR(100) NOT NULL"},
            {"gpa", "float"}
        };
    }
};

#endif //STUDENT_H
