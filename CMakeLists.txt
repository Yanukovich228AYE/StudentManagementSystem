cmake_minimum_required(VERSION 3.31)
project(StudentManagementSystem)

set(CMAKE_CXX_STANDARD 20)

include_directories(/usr/include/mysql)
include_directories(include)

link_directories(/usr/lib)

add_executable(StudentManagementSystem src/main.cpp
        include/student.h
        src/db.cpp
        src/db.h
        include/teacher.h)

target_link_libraries(StudentManagementSystem mariadb)