#pragma once 

#include <mysql/mysql.h>

extern thread_local std::unique_ptr<MYSQL> mysql;

void orihime_sql();
