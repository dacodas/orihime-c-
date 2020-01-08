#pragma once

#include "sql.h"

sql::mysql::MySQL_Driver* driver;
std::unique_ptr<sql::Connection> connection;

void orihime_sql()
{
    // Deleting this causes segmentation fault
    driver = {sql::mysql::get_mysql_driver_instance()};

    // TODO: Get more information from the connection for error
    connection = std::unique_ptr<sql::Connection> (driver->connect("tcp://10.0.2.100:3306", "root", "temporary-test-password"));
    if ( not connection->isValid() )
        throw std::logic_error("Couldn't connect to SQL database");

    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    statement->execute("USE orihime");
}
