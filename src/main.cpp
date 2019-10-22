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

#include "sql.hpp"
#include "dispatch.hpp"

void sql_orihime()
{
    // Deleting this causes segmentation fault
    driver = {sql::mysql::get_mysql_driver_instance()};

    connection = std::unique_ptr<sql::Connection> (driver->connect("tcp://127.0.0.1:3306", "dacoda", "dacoda"));
    connection->isValid();

    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    statement->execute("USE orihime");
}

int main(int argc, char* argv[])
{
    sql_orihime();

    std::streambuf * cin_streambuf  = std::cin.rdbuf();
    std::streambuf * cout_streambuf = std::cout.rdbuf();
    std::streambuf * cerr_streambuf = std::cerr.rdbuf();

    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while ( FCGX_Accept_r(&request) == 0 )
    {
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        std::cin.rdbuf(&cin_fcgi_streambuf);
        std::cout.rdbuf(&cout_fcgi_streambuf);
        std::cerr.rdbuf(&cerr_fcgi_streambuf);

        std::cout
            << "Content-type: application/json\r\n"
            << "\r\n";

        dispatch(std::move(request));
    }

    std::cin.rdbuf(cin_streambuf);
    std::cout.rdbuf(cout_streambuf);
    std::cerr.rdbuf(cerr_streambuf);

    return 0;
}

