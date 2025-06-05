#include "db.h"
#include "auth.h"
#include "User.h"
#include <iostream>

int main() {
    try {
        auto db = connect_to_db("localhost", "root", "root", "test");
        if (!db) return 1;

        std::cout << "Connected to DB\n";

        while (true) {
            std::cout << "\n1. Register\n2. Login\n3. Exit\nChoice: ";
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                std::string name, password, role_str;
                std::optional<float> gpa;
                std::optional<std::string> subject;

                std::cout << "Enter username: ";
                std::cin >> name;
                std::cout << "Enter password: ";
                std::cin >> password;
                std::cout << "Enter role (student/teacher/admin): ";
                std::cin >> role_str;
                UserRole role = User::string_to_role(role_str);

                if (role == Student) {
                    float g;
                    std::cout << "Enter GPA: ";
                    std::cin >> g;
                    gpa = g;
                } else if (role == Teacher) {
                    std::string subj;
                    std::cout << "Enter subject: ";
                    std::cin >> subj;
                    subject = subj;
                }

                User new_user(-1, name, password, gpa, subject, role);
                if (register_user(*db, new_user)) {
                    std::cout << "User registered successfully!\n";
                } else {
                    std::cout << "Registration failed.\n";
                }
            }

            else if (choice == 2) {
                std::string name, password;
                std::cout << "Enter username: ";
                std::cin >> name;
                std::cout << "Enter password: ";
                std::cin >> password;

                auto user_opt = login_user(*db, name, password);
                if (user_opt) {
                    User user = *user_opt;
                    std::cout << "Welcome back, " << user.get_name() << "!\n";

                    switch (user.get_role()) {
                        case Admin:
                            std::cout << "[ADMIN] You can manage all users.\n";
                            break;
                        case Teacher:
                            std::cout << "[TEACHER] You can view/edit students.\n";
                            break;
                        case Student:
                            std::cout << "[STUDENT] Your GPA: " << (user.get_gpa().has_value() ? std::to_string(user.get_gpa().value()) : "N/A") << "\n";
                            break;
                        default:
                            std::cout << "Unknown role.\n";
                    }

                } else {
                    std::cout << "Login failed.\n";
                }
            }

            else if (choice == 3) {
                break;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
