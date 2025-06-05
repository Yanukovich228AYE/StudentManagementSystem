
// main.cpp (updated)
#include "db.h"
#include "auth.h"
#include "User.h"
#include "management.h"  // Add this line
#include <iostream>

int main() {
    try {
        auto db = connect_to_db("localhost", "root", "root", "test");
        if (!db) return 1;

        std::cout << "Connected to DB\n";

        // Then create fresh tables
        User temp_user;
        if (!create_table(*db, "test", "users", temp_user.get_schema())) {
            return 1;
        }

        while (true) {
            std::cout << "\n1. Register\n2. Login\n3. Exit\nChoice: ";
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                std::string name, password, role_str;
                std::optional<double> gpa;
                std::optional<std::string> subject;

                std::cout << "Enter username: ";
                std::cin >> name;
                std::cout << "Enter password: ";
                std::cin >> password;
                std::cout << "Enter role (student/teacher/admin): ";
                std::cin >> role_str;
                UserRole role = User::string_to_role(role_str);

                if (role == Student) {
                    double g;
                    std::cout << "Enter GPA: ";
                    std::cin >> g;
                    gpa = g;
                    subject = std::nullopt;
                } else if (role == Teacher) {
                    std::string subj;
                    std::cout << "Enter subject: ";
                    std::cin >> subj;
                    subject = subj;
                    gpa = std::nullopt;
                }

                User new_user(-1, name, password, gpa, subject, role);
                if (register_user(*db, new_user)) {
                    std::cout << "User registered successfully!\n";
                } else {
                    // Check if it's a duplicate error
                    auto existing = search_db(*db, "users", "name", new_user.get_name());
                    if (existing) {
                        std::cout << "Registration failed: Username already exists.\n";
                    } else {
                        std::cout << "Registration failed for unknown reason.\n";
                    }
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
                            admin_menu(*db, user);
                            break;
                        case Teacher:
                            teacher_menu(*db, user);
                            break;
                        case Student:
                            student_menu(*db, user);
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
