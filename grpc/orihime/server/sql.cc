#pragma once

#include "sql.h"

std::unique_ptr<MYSQL> mysql;

void orihime_sql()
{

    mysql = { mysql_init(nullptr) }

    if ( not 
    mysql_real_connect(mysql,
		    "127.0.0.1",
		    "root",
		    "dacodastrackoda",
		    "orihime",
		    3306,
		    NULL,
		    CLIENT_FOUND_ROWS) )
    {
	    std::cerr << "Failed to connect to database\n";
	    exit(1);
    }

    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    statement->execute("USE orihime");
}
