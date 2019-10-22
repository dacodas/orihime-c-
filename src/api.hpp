#include <string>
#include <vector>

#include "sql.hpp"
#include "json.hpp"

void sources_GET(const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    std::unique_ptr<sql::ResultSet> result {statement->executeQuery("SELECT * FROM source")};

    // output_array_from_sql(result);
}

void sources_HEAD(const std::vector<std::string>& parameters) {} 
void sources_POST(const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement("INSERT INTO source (name) VALUES (?)")};
    statement->setString(1, parameters[0]);

    try
    {
        statement->execute();
    }
    catch (const std::exception& exception)
    {
        std::cout << "Failure\n";
        std::cerr << exception.what();
    }
}

void sources_id_GET(const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement("SELECT * FROM source WHERE id = ?")};
    statement->setString(1, parameters[0]);

    std::unique_ptr<sql::ResultSet> result {statement->executeQuery()};
    output_array_from_sql(result);
}

void sources_id_HEAD(const std::vector<std::string>& parameters) {} 
void texts_GET(const std::vector<std::string>& parameters) {} 
void texts_HEAD(const std::vector<std::string>& parameters) {} 
void texts_id_GET(const std::vector<std::string>& parameters) {} 
void texts_id_HEAD(const std::vector<std::string>& parameters) {} 
void words_GET(const std::vector<std::string>& parameters) {} 
void words_HEAD(const std::vector<std::string>& parameters) {} 
void words_id_GET(const std::vector<std::string>& parameters) {} 
void words_id_HEAD(const std::vector<std::string>& parameters) {} 
void users_id_texts_GET(const std::vector<std::string>& parameters) {} 
void users_id_texts_HEAD(const std::vector<std::string>& parameters) {} 
void users_id_texts_POST(const std::vector<std::string>& parameters) {} 
void users_id_texts_id_GET(const std::vector<std::string>& parameters) {} 
void users_id_texts_id_HEAD(const std::vector<std::string>& parameters) {} 
void users_id_texts_id_PUT(const std::vector<std::string>& parameters) {} 
void users_id_words_GET(const std::vector<std::string>& parameters) {} 
void users_id_words_HEAD(const std::vector<std::string>& parameters) {} 
void users_id_words_POST(const std::vector<std::string>& parameters) {} 
void users_id_words_id_GET(const std::vector<std::string>& parameters) {} 
void users_id_words_id_HEAD(const std::vector<std::string>& parameters) {} 
void users_id_words_id_PUT(const std::vector<std::string>& parameters) {} 
