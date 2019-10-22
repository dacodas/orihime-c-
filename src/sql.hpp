#pragma once 

#include <mysql_driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

sql::mysql::MySQL_Driver* driver;
thread_local std::unique_ptr<sql::Connection> connection;
