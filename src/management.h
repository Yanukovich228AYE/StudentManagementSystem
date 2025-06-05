// management.h
#ifndef MANAGEMENT_H
#define MANAGEMENT_H

#include "db.h"
#include "auth.h"
#include "User.h"
#include <iostream>

void print_users(soci::session& db, const User& current_user);
bool add_user(soci::session& db, const User& current_user);
bool remove_user(soci::session& db, const User& current_user);
void admin_menu(soci::session& db, User& admin);
void teacher_menu(soci::session& db, User& teacher);
void student_menu(soci::session& db, User& student);

#endif // MANAGEMENT_H
