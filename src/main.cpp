#include <fstream>
#include <iostream>
#include <regex>
#include <functional>
#include <vector>
#include <string>

// http://rapidjson.org/md_doc_tutorial.html
// https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-examples-results.html
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>

#include <fcgio.h>

#include "config.h"
#include "sql.hpp"

int main(int argc, char* argv[])
{
    sql_orihime();

    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    mainLoop(request);

    return 0;
}

