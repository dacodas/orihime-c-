#pragma once 

// https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-examples-results.html
#include <mysql/mysql.h>
#include <mysql_driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

// extern thread_local std::unique_ptr<sql::Connection> connection;
extern std::unique_ptr<sql::Connection> connection;

void orihime_sql();


