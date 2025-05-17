cmake_minimum_required(VERSION 3.31)
project(StudentManagementSystem)

set(CMAKE_CXX_STANDARD 20)

include_directories(/usr/include/mysql)
include_directories(/usr/include/openssl)
include_directories(/usr/include)
include_directories(include)

link_directories(/usr/lib)

find_package(OpenSSL REQUIRED)

add_executable(StudentManagementSystem src/main.cpp
        src/db.cpp
        src/db.h
        include/User.h)

target_link_libraries(StudentManagementSystem
        mariadb
        OpenSSL::Crypto
        OpenSSL::SSL
        soci_core
        soci_mysql
)

target_include_directories(StudentManagementSystem PRIVATE /usr/include/soci)