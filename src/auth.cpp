#include "libs.h"
#include "auth.h"
#include "db.h"

bool register_user(soci::session& db, const User& user) {
    User hashed(
        user.get_id(),
        user.get_name(),
        hash(user.get_password()),
        user.get_gpa(),
        user.get_subject(),
        user.get_role());
    return insert_user(db, "users", hashed);
}


std::optional<User> login_user(soci::session& db, const std::string& name, const std::string& password) {
    int id, role_int;
    std::string db_password; // This stores the hashed password from DB
    soci::indicator gpa_ind, subject_ind;
    float gpa;
    std::string subject;

    try {
        soci::statement st = (db.prepare <<
            "SELECT id, password, role, gpa, subject FROM users WHERE name = :name",
            soci::into(id),
            soci::into(db_password),
            soci::into(role_int),
            soci::into(gpa, gpa_ind),
            soci::into(subject, subject_ind),
            soci::use(name)
        );

        st.execute(true);

        // Compare hashed input with stored hash
        if (db_password == hash(password)) {
            std::optional<float> opt_gpa;
            std::optional<std::string> opt_subject;

            if (gpa_ind == soci::i_ok) opt_gpa = gpa;
            if (subject_ind == soci::i_ok) opt_subject = subject;

            // Return user with hashed password (not the plaintext one)
            User user(id, name, db_password, opt_gpa, opt_subject, static_cast<UserRole>(role_int));
            return user;
        }
    } catch (const std::exception& e) {
        std::cerr << "Login failed: " << e.what() << std::endl;
    }

    return std::nullopt;
}