#pragma once 

#include <mysql_driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

sql::mysql::MySQL_Driver* driver;
thread_local std::unique_ptr<sql::Connection> connection;

void sql_orihime()
{
    // Deleting this causes segmentation fault
    driver = {sql::mysql::get_mysql_driver_instance()};

    connection = std::unique_ptr<sql::Connection> (driver->connect("tcp://127.0.0.1:3306", "dacoda", "dacoda"));
    connection->isValid();

    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    statement->execute("USE orihime");
}

