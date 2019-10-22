#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

int main()
{
    std::unique_ptr<sql::mysql::MySQL_Driver> driver {sql::mysql::get_mysql_driver_instance()};

    {
        std::unique_ptr<sql::Connection> connection {driver->connect("tcp://127.0.0.1:3306", "dacoda", "dacoda")};

        connection->isValid();

        {
            std::unique_ptr<sql::Statement> statement {connection->createStatement()};
            statement->execute("USE orihime");
        }

        {
            std::unique_ptr<sql::PreparedStatement> prepared_statement {connection->prepareStatement("INSERT INTO source(name) VALUES (?)")};
            // prepared_statement->setString(1, "goo辞書");
            prepared_statement->setString(1, "Life of Dacoda");
            prepared_statement->execute();
        }
    }

    return 0;
}
