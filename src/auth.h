#ifndef AUTH_H
#define AUTH_H

#include "libs.h"

bool register_user(soci::session& db, const User& user);
std::optional<User> login_user(soci::session& db, const std::string& name, const std::string& password);

#endif //AUTH_H
