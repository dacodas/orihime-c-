#include <string>
#include <vector>
#include <queue>
#include <tuple>

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

std::string body_to_string(const OrihimeRequest& request)
{
    int content_length {std::stoi(request.parameter("CONTENT_LENGTH"))};
    std::string document_string(content_length, ' ');
    rin.read(document_string.data(), content_length);

    return document_string;
}

rapidjson::Document parse_body(const OrihimeRequest& request)
{ 
    rapidjson::Document document;
    std::string document_string = body_to_string(request);
    document.Parse(document_string.c_str());

    return document;
}

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

void serialize_object(std::unique_ptr<sql::ResultSet> result, const std::vector<std::string>& fields)
{
    rapidjson::Document document {};
    document.SetArray();

    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    while ( result->next() )
    {
        rapidjson::Value object;
        object.SetObject();

        for ( const auto& field : fields )
        {
            sql::SQLString string {result->getString(field)};

            rapidjson::Value field_value;
            field_value.SetString(string.c_str(), string.asStdString().size(), allocator);

            rapidjson::Value().SetString(string.c_str(), string.asStdString().size(), allocator);

            object.AddMember(
                    rapidjson::Value().SetString(rapidjson::GenericStringRef {field.c_str()}),
                    field_value, document.GetAllocator());
        }

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

    rapidjson::Document document {parse_body(request)};

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

        // Now find and return the result for the user? Let's learn
        // more about the result of execute()
        // sources_id_GET(std::move(request), parameters);

        std::cout << "Success\n";
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

void okay()
{
    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    ;
}

void sources_id_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void texts_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) 
{
    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    
    std::vector<std::string> fields {"user", "source", "contents"};
    // std::unique_ptr<sql::ResultSet> result {statement->executeQuery("SELECT * FROM text")};

    std::unique_ptr<sql::ResultSet> result {
        statement->executeQuery(
            R"sql(
SELECT user.name     AS user,
       source.name   AS source,
       text.contents AS contents
FROM text
INNER JOIN source ON source.id = text.source
INNER JOIN user ON user.id = text.user;
)sql")};

    serialize_object(std::move(result), fields);
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
void users_id_texts_POST(OrihimeRequest&& request, const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement(
            R"sql(
CALL add_text(?)
)sql")};

    statement->setString(1, body_to_string(request));
    std::unique_ptr<sql::ResultSet> result {statement->executeQuery()};
}
void users_id_texts_id_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement(
            R"sql(
CALL orihime.TextTreeJSON(?, ?);
)sql")};

    statement->setString(1, request.parameter("USER"));
    statement->setString(2, request.parameter("TEXT"));
    std::unique_ptr<sql::ResultSet> result {statement->executeQuery()};

    // A regular pointer, we are modifying this value, but ownership
    // is clearly defined by the document and it remains in scope this
    // entire time
    //
    // Consider using std::array<int, 32>
    rapidjson::Document root_text;

    using HashToObject = std::pair<std::string, rapidjson::Value*>;
    std::vector<HashToObject> current_text_locations;
    {
        result->next();
        sql::SQLString root_hash = result->getString("hash");
        sql::SQLString root_text_json = result->getString("children");

        if ( strcmp(root_hash.c_str(), "00000000000000000000000000000000") )
            throw std::logic_error("The hash of the root text returned from SQL is expected to be all zeros");

        root_text.Parse(root_text_json.c_str());

        if ( not root_text.IsObject()
             or not root_text.GetObject().HasMember("id")
             or not root_text.GetObject().HasMember("contents")
             or not root_text.GetObject().HasMember("source")
             or not root_text.GetObject()["id"].IsString() )
            throw std::logic_error("");

        current_text_locations.emplace_back(HashToObject {root_text.GetObject()["id"].GetString(), &root_text});
    }

    using ParentHashAndChildren = std::pair<std::string, rapidjson::Document>;
    std::queue<ParentHashAndChildren> children_to_insert {};
    while ( result->next() )
    {
        rapidjson::Document children(&root_text.GetAllocator());
        sql::SQLString parent_hash = result->getString("hash");
        sql::SQLString children_json = result->getString("children");

        children.Parse(children_json.c_str());
        if ( not children.IsArray() )
            throw std::logic_error("");

        children_to_insert.emplace(ParentHashAndChildren {std::string(parent_hash.c_str()), std::move(children)});
    }

    while ( not children_to_insert.empty() )
    {
        ParentHashAndChildren current = std::move(children_to_insert.front());

        auto& [parent_hash, children] = current;

        std::vector<HashToObject>::iterator result =
            std::find_if(
                current_text_locations.begin(),
                current_text_locations.end(),
                [&](const HashToObject& a) { return a.first == parent_hash; }
                );

        if ( result == current_text_locations.end() )
        {
            children_to_insert.emplace(std::move(current));
        }
        else
        {
            result->second->AddMember("children", children, root_text.GetAllocator());
            rapidjson::Value& children_array = (*result->second)["children"];

            for ( rapidjson::Value::ValueIterator it = children_array.Begin() ;
                  it != children_array.End() ;
                  ++it )
            {
                current_text_locations.emplace_back(HashToObject {it->GetObject()["id"].GetString(), &(*it)});
            }
        }

        children_to_insert.pop();
    }

    rapidjson::OStreamWrapper wrapper {std::cout};
    rapidjson::Writer<rapidjson::OStreamWrapper> writer {wrapper};
    root_text.Accept(writer);
} 

void users_id_texts_id_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_texts_id_PUT(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_texts_id_POST(OrihimeRequest&& request, const std::vector<std::string>& parameters)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement(
            R"sql(
CALL orihime.add_child_word(?, ?, ?);
)sql")};

    statement->setString(1, body_to_string(request));
    statement->setString(2, request.parameter("USER"));
    statement->setString(3, request.parameter("TEXT"));
    std::unique_ptr<sql::ResultSet> result {statement->executeQuery()};

}
void users_id_words_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_POST(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_id_GET(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_id_HEAD(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
void users_id_words_id_PUT(OrihimeRequest&& request, const std::vector<std::string>& parameters) {} 
