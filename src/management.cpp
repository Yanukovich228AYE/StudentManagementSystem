#include "management.h"
#include <limits>

void print_users_based_on_role(soci::session& db, const User& current_user) {
    try {
        soci::rowset<soci::row> rs = (db.prepare << "SELECT id, name, role FROM users");

        std::cout << "\nUser List:\n";
        std::cout << "ID\tName\tRole\n";
        std::cout << "----------------------------\n";

        for (const auto& row : rs) {
            int id = row.get<int>(0);
            std::string name = row.get<std::string>(1);
            UserRole role = static_cast<UserRole>(row.get<int>(2));

            // Students can only see names and roles
            if (current_user.get_role() == Student) {
                std::cout << id << "\t" << name << "\t" << User::role_to_string(role) << "\n";
            }
            // Teachers and admins can see all details
            else {
                // For simplicity, we're just showing basic info here
                // You could expand this to show more details
                std::cout << id << "\t" << name << "\t" << User::role_to_string(role) << "\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error printing users: " << e.what() << std::endl;
    }
}

bool add_user_based_on_role(soci::session& db, const User& current_user) {
    if (current_user.get_role() == Student) {
        std::cout << "Students cannot add users.\n";
        return false;
    }

    std::string name, password, role_str;
    std::optional<double> gpa;
    std::optional<std::string> subject;

    std::cout << "Enter username: ";
    std::cin >> name;
    std::cout << "Enter password: ";
    std::cin >> password;

    // Teachers can only add students
    if (current_user.get_role() == Teacher) {
        role_str = "student";
        std::cout << "Enter GPA: ";
        double g;
        std::cin >> g;
        gpa = g;
    }
    // Admins can add any role
    else if (current_user.get_role() == Admin) {
        std::cout << "Enter role (student/teacher/admin): ";
        std::cin >> role_str;

        if (User::string_to_role(role_str) == Student) {
            std::cout << "Enter GPA: ";
            double g;
            std::cin >> g;
            gpa = g;
        } else if (User::string_to_role(role_str) == Teacher) {
            std::cout << "Enter subject: ";
            std::string subj;
            std::cin >> subj;
            subject = subj;
        }
    }

    UserRole role = User::string_to_role(role_str);
    User new_user(-1, name, password, gpa, subject, role);

    if (register_user(db, new_user)) {
        std::cout << "User added successfully!\n";
        return true;
    } else {
        std::cout << "Failed to add user.\n";
        return false;
    }
}

bool remove_user_based_on_role(soci::session& db, const User& current_user) {
    if (current_user.get_role() == Student) {
        std::cout << "Students cannot remove users.\n";
        return false;
    }

    int id;
    std::cout << "Enter user ID to remove: ";
    std::cin >> id;

    try {
        // First get the user to check their role
        soci::row row;
        db << "SELECT role FROM users WHERE id = :id", soci::use(id), soci::into(row);

        UserRole target_role = static_cast<UserRole>(row.get<int>(0));

        // Teachers can only remove students
        if (current_user.get_role() == Teacher && target_role != Student) {
            std::cout << "Teachers can only remove students.\n";
            return false;
        }

        // Admins can remove anyone except themselves
        if (current_user.get_role() == Admin && id == current_user.get_id()) {
            std::cout << "Cannot remove yourself.\n";
            return false;
        }

        db << "DELETE FROM users WHERE id = :id", soci::use(id);
        std::cout << "User removed successfully.\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error removing user: " << e.what() << std::endl;
        return false;
    }
}

void admin_menu(soci::session& db, User& admin) {
    while (true) {
        std::cout << "\nADMIN MENU\n";
        std::cout << "1. View all users\n";
        std::cout << "2. Add user\n";
        std::cout << "3. Remove user\n";
        std::cout << "4. Logout\n";
        std::cout << "Choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                print_users_based_on_role(db, admin);
                break;
            case 2:
                add_user_based_on_role(db, admin);
                break;
            case 3:
                remove_user_based_on_role(db, admin);
                break;
            case 4:
                return;
            default:
                std::cout << "Invalid choice.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void teacher_menu(soci::session& db, User& teacher) {
    while (true) {
        std::cout << "\nTEACHER MENU\n";
        std::cout << "1. View all users\n";
        std::cout << "2. Add student\n";
        std::cout << "3. Remove student\n";
        std::cout << "4. Logout\n";
        std::cout << "Choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                print_users_based_on_role(db, teacher);
                break;
            case 2:
                {
                    User temp(-1, "", "", std::nullopt, std::nullopt, Student);
                    add_user_based_on_role(db, teacher);
                }
                break;
            case 3:
                remove_user_based_on_role(db, teacher);
                break;
            case 4:
                return;
            default:
                std::cout << "Invalid choice.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void student_menu(soci::session& db, User& student) {
    while (true) {
        std::cout << "\nSTUDENT MENU\n";
        std::cout << "1. View users\n";
        std::cout << "2. View my info\n";
        std::cout << "3. Logout\n";
        std::cout << "Choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                print_users_based_on_role(db, student);
                break;
            case 2:
                {
                    std::cout << "\nYour Information:\n";
                    std::cout << "Name: " << student.get_name() << "\n";
                    std::cout << "Role: Student\n";
                    if (student.get_gpa().has_value()) {
                        std::cout << "GPA: " << student.get_gpa().value() << "\n";
                    }
                }
                break;
            case 3:
                return;
            default:
                std::cout << "Invalid choice.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}
