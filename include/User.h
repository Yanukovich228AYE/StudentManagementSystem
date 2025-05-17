#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <utility>
#include <optional>

enum UserRole {
    Student = 0,
    Teacher = 1,
    Admin = 2,
    Unknown = -1
};

class User {
private:
    std::string name;
    std::string password;
    std::optional<float> gpa;
    std::optional<std::string> subject;
    UserRole role = UserRole::Student;

public:
    User() = default; // tell the compiler to generate desctructor automatically

    User(std::string name, std::string password, UserRole role, std::optional<float> gpa = std::nullopt, std::optional<std::string> subject = std::nullopt)
        : name(std::move(name)), password(std::move(password)), gpa(gpa), subject(subject), role(role) {}

    // Accessors
    const std::string& get_name() const { return name; }
    const std::string& get_password() const { return password; }
    std::optional<float> get_gpa() const { return gpa; }
    std::optional<std::string> get_subject() const { return subject; }
    UserRole get_role() const { return role; }

    void set_id(int new_id) { id = new_id; }
    int get_id() const { return id; }

    const std::vector<std::string>& get_columns() {
        return {"name", "password", "gpa", "subject", "role"};
    }

    const std::vector<std::string>& get_values() {
        return {
            ""
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
};

#endif //ENTITY_H
