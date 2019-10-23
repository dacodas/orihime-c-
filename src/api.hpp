#include <string>
#include <vector>

#include <fcgiapp.h>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include "sql.hpp"

// Let's consider how to serialize these. For now the modus operandi will be. 
// 
// 1. Grab a single row
// 2. Instantiate JSON object
// 3. For each field 
//   1. Object field keys will reference std::vector<std::string>> fields
//   2. Object field values will be copied from the row 
// 4. Push JSON object to document
//
// Perhaps we can process rows in blocks, bringing them into memory,
// referencing everything, copying nothing, outputting to std::out and
// then moving on to the next block. 
//
// NEVER JOIN HERE ALWAYS JOIN IN SQL THIS SHOULD ONLY BE SIMPLE SERIALIZATION

void serialize_sources(std::unique_ptr<sql::ResultSet> result)
{
    rapidjson::Document document {};
    document.SetArray();

    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    while ( result->next() )
    {
        sql::SQLString string {result->getString("name")};

        rapidjson::Value name;
        name.SetString(string.c_str(), string.asStdString().size(), allocator);

        rapidjson::Value object;
        object.SetObject().AddMember("name", name, document.GetAllocator());

        document.PushBack(object.Move(), allocator);
    }

    rapidjson::OStreamWrapper wrapper {std::cout};
    rapidjson::Writer<rapidjson::OStreamWrapper> writer {wrapper};
    document.Accept(writer);
}

void sources_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    std::unique_ptr<sql::ResultSet> result {statement->executeQuery("SELECT * FROM source")};
    serialize_sources(std::move(result));
}

void sources_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void sources_POST(OrihimeRequest&& request, const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement("INSERT INTO source (name) VALUES (?)")};

    rapidjson::Document document;
    int content_length {std::stoi(request.parameter("CONTENT_LENGTH"))};

    std::string document_string(content_length, ' ');
    rin.read(document_string.data(), content_length);
    document.Parse(document_string.c_str());

    if ( not document.IsString() )
    {
        rerr << "That is not a JSON string\n";
        return;
    }

    // Use jq for validating data?
    statement->setString(1, document.GetString());

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
    serialize_sources(std::move(result));
}

void sources_id_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void texts_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) 
{
    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    std::unique_ptr<sql::ResultSet> result {statement->executeQuery("SELECT * FROM text")};

    std::vector<std::string> fields {"id", "user", "source", "contents"};

    {
        rapidjson::Document document {};
        document.SetArray();

        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

        while ( result->next() )
        {
            for ( const auto& field : fields )
            {
                sql::SQLString string {result->getString(field)};

                rapidjson::Value field_value;
                field_value.SetString(string.c_str(), string.asStdString().size(), allocator);

                rapidjson::Value().SetString(string.c_str(), string.asStdString().size(), allocator);

                rapidjson::Value object;
                object
                    .SetObject()
                    .AddMember(
                        rapidjson::Value().SetString(rapidjson::GenericStringRef {field.c_str()}),
                        field_value, document.GetAllocator());

                document.PushBack(object.Move(), allocator);
            }
        }

        rapidjson::OStreamWrapper wrapper {std::cout};
        rapidjson::Writer<rapidjson::OStreamWrapper> writer {wrapper};
        document.Accept(writer);
    }
}

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
