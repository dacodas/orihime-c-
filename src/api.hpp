#include <string>
#include <vector>

#include <rapidjson/istreamwrapper.h>
#include <fcgiapp.h>

#include "sql.hpp"
#include "json.hpp"


void sources_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    std::unique_ptr<sql::ResultSet> result {statement->executeQuery("SELECT * FROM source")};

    output_array_from_sql(result);
}

void sources_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void sources_POST(OrihimeRequest&& request, const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement("INSERT INTO source (name) VALUES (?)")};

    rapidjson::Document document;
    request.envp;
    // rapidjson::InputStream stream rapidjson::BasicIStreamWrapper(std::cin)
    // document.ParseStream(stream);

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

void sources_id_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement("SELECT * FROM source WHERE id = ?")};
    statement->setString(1, parameters[0]);

    std::unique_ptr<sql::ResultSet> result {statement->executeQuery()};
    output_array_from_sql(result);
}

void sources_id_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void texts_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void texts_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void texts_id_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void texts_id_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void words_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void words_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void words_id_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void words_id_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_texts_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_texts_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_texts_POST(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_texts_id_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_texts_id_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_texts_id_PUT(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_POST(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_id_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_id_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_id_PUT(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
