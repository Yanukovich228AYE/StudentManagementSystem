#ifndef USER_H
#define USER_H

#include "libs.h"

enum UserRole {
    Student = 0,
    Teacher = 1,
    Admin = 2,
    Unknown = -1
};

class User {
private:
    int id = -1;
    std::string name;
    std::string password;
    std::optional<double> gpa;
    std::optional<std::string> subject;
    UserRole role;

public:
    User() = default; // tell the compiler to generate desctructor automatically

    User(int id = -1,
     std::string name = "",
     std::string password = "",
     std::optional<double> gpa = std::nullopt,
     std::optional<std::string> subject = std::nullopt,
     UserRole role = Unknown)
    : id(id), name(std::move(name)), password(std::move(password)),
      gpa(gpa), subject(subject), role(role) {}

    // Accessors
    const std::string& get_name() const { return name; }
    const std::string& get_password() const { return password; }
    std::optional<double> get_gpa() const { return gpa; }
    std::optional<std::string> get_subject() const { return subject; }
    UserRole get_role() const { return role; }

    void set_id(int new_id) { id = new_id; }
    int get_id() const { return id; }

    std::vector<std::string> get_columns() const {
        return {"id", "name", "password", "gpa", "subject", "role"};
    }

    std::vector<std::string> get_values() const {
        return {
            std::to_string(id),
            name,
            password,
            gpa.has_value() ? std::to_string(gpa.value()) : "NULL",
            subject.has_value() ? subject.value() : "NULL",
            std::to_string(static_cast<int>(role))
        };
    }

    std::vector<std::pair<std::string, std::string>> get_schema() const {
        return {
                {"id", "INT PRIMARY KEY AUTO_INCREMENT"},
                {"name", "VARCHAR(50) NOT NULL UNIQUE"},
                {"password", "VARCHAR(256) NOT NULL"},
                {"gpa", "FLOAT"}, // can be null for teachers/admins
                {"subject", "VARCHAR(50)"}, // can be null for students/admins
                {"role", "INT NOT NULL"}
        };
    }

    static std::string role_to_string(UserRole role) {
        switch (role) {
            case Student: return "Student";
            case Teacher: return "Teacher";
            case Admin: return "Admin";
            default: return "Unknown";
        }
    }

    static UserRole string_to_role(const std::string& str) {
        if (str == "Student" || str == "student") return Student;
        if (str == "Teacher" || str == "teacher") return Teacher;
        if (str == "Admin" || str == "admin") return Admin;
        return Unknown;
    }
};

#endif //USER_H
