#include <queue>
#include <tuple>

#include <curl/curl.h>
#include <fcgiapp.h>

// http://rapidjson.org/md_doc_tutorial.html
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include "sql.h"
#include "sanitize.h"
#include "OrihimeRequest.h"

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
    request.rin->read(document_string.data(), content_length);

    return document_string;
}

rapidjson::Document parse_body(const OrihimeRequest& request)
{ 
    rapidjson::Document document;
    std::string document_string = body_to_string(request);
    document.Parse(document_string.c_str()); 

    return document;
}

void serialize_sources(std::unique_ptr<sql::ResultSet> result, std::ostream& stream)
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

    rapidjson::OStreamWrapper stream_wrapper(stream);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer {stream_wrapper};
    document.Accept(writer);
}

void serialize_object(std::unique_ptr<sql::ResultSet> result, const std::vector<std::string>& fields, std::ostream& output)
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

    rapidjson::OStreamWrapper wrapper {output};
    rapidjson::Writer<rapidjson::OStreamWrapper> writer {wrapper};
    document.Accept(writer);
}

void sources_GET(OrihimeRequest& request)
{
    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    std::unique_ptr<sql::ResultSet> result {statement->executeQuery("SELECT * FROM source")};
    serialize_sources(std::move(result), *request.rout);
}

void sources_HEAD(OrihimeRequest& request) {} 
void sources_POST(OrihimeRequest& request)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement("INSERT INTO source (name) VALUES (?)")};

    rapidjson::Document document {parse_body(request)};

    if ( not document.IsString() )
    {
        *request.rerr << "That is not a JSON string\n";

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

        *request.rout << "Success\n";
    }
    catch (const std::exception& exception)
    {
        *request.rout << "Failure\n";
        *request.rerr << exception.what();
    }
}

void sources_id_GET(OrihimeRequest& request)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement("SELECT * FROM source WHERE id = ?")};
    statement->setString(1, request.parameter("SOURCE"));

    std::unique_ptr<sql::ResultSet> result {statement->executeQuery()};
    serialize_sources(std::move(result), *request.rout);
}

void sources_id_HEAD(OrihimeRequest& request) {} 
void texts_GET(OrihimeRequest& request) 
{
    std::unique_ptr<sql::Statement> statement {connection->createStatement()};
    
    std::vector<std::string> fields {"source", "contents"};

    std::unique_ptr<sql::ResultSet> result {
        statement->executeQuery(
            R"sql(
SELECT source.name   AS source,
       text.contents AS contents
FROM text
INNER JOIN source ON source.id = text.source;
)sql")};

    serialize_object(std::move(result), fields, *request.rout);
}

void texts_HEAD(OrihimeRequest& request) {} 
void texts_id_GET(OrihimeRequest& request) {} 
void texts_id_HEAD(OrihimeRequest& request) {} 
void words_GET(OrihimeRequest& request) {} 
void words_HEAD(OrihimeRequest& request) {} 
void words_id_GET(OrihimeRequest& request) {} 
void words_id_HEAD(OrihimeRequest& request) {} 
void users_id_texts_GET(OrihimeRequest& request) {} 
void users_id_texts_HEAD(OrihimeRequest& request) {} 
void users_id_texts_POST(OrihimeRequest& request)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement(
            R"sql(
CALL add_text(?)
)sql")};

    statement->setString(1, body_to_string(request));
    std::unique_ptr<sql::ResultSet> result {statement->executeQuery()};
}

// Exchange contents for better memory usage
// https://en.cppreference.com/w/cpp/string/basic_string/data
// CharT* data() noexcept; since C++17
// xmlSetProp(root_div, "id", std::exchange(hash.data(), nullptr));

xmlNodePtr root_text_from_sql(std::shared_ptr<sql::ResultSet> result)
{
    sql::SQLString id = result->getString("id");
    sql::SQLString word = result->getString("word");
    sql::SQLString contents = result->getString("definition");
    sql::SQLString source = result->getString("source");

    xmlNodePtr word_div = xmlNewNode(nullptr, (const xmlChar*) "div");
    xmlNewProp(word_div, (const xmlChar*) "class", (const xmlChar*) "orihime-text");
    xmlNewProp(word_div, (const xmlChar*) "id", (const xmlChar*) id.c_str());
    xmlNodePtr contents_div = xmlNewChild(word_div, nullptr, (const xmlChar*) "div", nullptr);
    xmlNewProp(contents_div, (const xmlChar*) "class", (const xmlChar*) "orihime-contents");
    xmlNodePtr source_div = xmlNewChild(word_div, nullptr, (const xmlChar*) "div", (const xmlChar*) source.c_str());
    xmlNewProp(source_div, (const xmlChar*) "class", (const xmlChar*) "orihime-source");

    htmlDocPtr contents_document =
        htmlReadDoc((const xmlChar*) contents.c_str(),
                    nullptr, "utf-8",
                    HTML_PARSE_NODEFDTD | HTML_PARSE_NONET | HTML_PARSE_NOIMPLIED);

    int indent = 0;
    xmlNodePtr contents_root = xmlDocGetRootElement(contents_document);
    filter(contents_root, indent);

    xmlUnlinkNode(contents_root);
    xmlAddChild(contents_div, contents_root);
    xmlFreeDoc(contents_document);

    return word_div;
}

xmlNodePtr child_word_from_sql(std::shared_ptr<sql::ResultSet> result)
{
    sql::SQLString id = result->getString("id");
    sql::SQLString word = result->getString("word");
    sql::SQLString definition = result->getString("definition");
    sql::SQLString source = result->getString("source");

    xmlNodePtr word_div = xmlNewNode(nullptr, (const xmlChar*) "div");
    xmlNewProp(word_div, (const xmlChar*) "class", (const xmlChar*) "orihime-word");
    xmlNewProp(word_div, (const xmlChar*) "id", (const xmlChar*) id.c_str());
    xmlNodePtr reading_div = xmlNewChild(word_div, nullptr, (const xmlChar*) "div", (const xmlChar*) word.c_str());
    xmlNewProp(reading_div, (const xmlChar*) "class", (const xmlChar*) "orihime-reading");
    xmlNodePtr definition_div = xmlNewChild(word_div, nullptr, (const xmlChar*) "div", nullptr);
    xmlNewProp(definition_div, (const xmlChar*) "class", (const xmlChar*) "orihime-definition");
    xmlNodePtr source_div = xmlNewChild(word_div, nullptr, (const xmlChar*) "div", (const xmlChar*) source.c_str());
    xmlNewProp(source_div, (const xmlChar*) "class", (const xmlChar*) "orihime-source");

    htmlDocPtr definition_document =
        htmlReadDoc((const xmlChar*) definition.c_str(),
                    nullptr, "utf-8",
                    HTML_PARSE_NODEFDTD | HTML_PARSE_NONET | HTML_PARSE_NOIMPLIED);

    int indent = 0;
    xmlNodePtr definition_root = xmlDocGetRootElement(definition_document);
    filter(definition_root, indent);

    xmlUnlinkNode(definition_root);
    xmlAddChild(definition_div, definition_root);
    xmlFreeDoc(definition_document);

    return word_div;
}

void insert_as_child(xmlNodePtr parent, xmlNodePtr child)
{
    if ( strcmp((const char*) xmlGetProp(parent->last, (const xmlChar*) "class"), "orihime-children") == 0 )
    {
        xmlAddChild(parent->last, child);
    }
    else
    {
        xmlNodePtr children_node = xmlNewNode(nullptr, (const xmlChar*) "div");
        xmlNewProp(children_node, (const xmlChar*) "class", (const xmlChar*) "orihime-children");
        xmlAddChild(children_node, child);
        xmlAddChild(parent, children_node);
    }
}

// TODO: Abstract and consolidate the HTML and JSON versions of this function 
// void users_id_texts_id_GET_HTML(OrihimeRequest& request)
void users_id_texts_id_GET(OrihimeRequest& request)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement(
            R"sql(
CALL orihime.TextTree(?, ?);
)sql")};

    statement->setString(1, request.parameter("USER"));
    statement->setString(2, request.parameter("TEXT"));
    std::shared_ptr<sql::ResultSet> result {statement->executeQuery()};

    using HashToNode = std::pair<std::string, xmlNodePtr>;
    using ParentHashAndNode = std::pair<std::string, xmlNodePtr>;

    std::vector<HashToNode> current_text_locations;
    std::queue<ParentHashAndNode> children_to_insert {};

    htmlDocPtr orihime_text_document = htmlNewDocNoDtD(nullptr, nullptr);

    while ( result->next() )
    {
        sql::SQLString hash = result->getString("hash");

        if ( strcmp(hash.c_str(), "00000000000000000000000000000000") == 0 )
        {
            xmlNodePtr root_element = root_text_from_sql(result);
            xmlDocSetRootElement(orihime_text_document, root_element);
            current_text_locations.emplace_back(HashToNode {result->getString("id").c_str(), root_element});
        }
        else
        {
            children_to_insert.emplace(ParentHashAndNode {std::string(hash.c_str()), child_word_from_sql(result)});
        }
    }

    // https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-tutorials-stored-routines-statement-with-result.html
    while ( statement->getMoreResults() ) {}

    while ( not children_to_insert.empty() )
    {
        ParentHashAndNode current = std::move(children_to_insert.front());

        auto& [parent_hash, child] = current;

        std::vector<HashToNode>::iterator result =
            std::find_if(
                current_text_locations.begin(),
                current_text_locations.end(),
                [&](const HashToNode & a) { return a.first == parent_hash; }
                );

        if ( result == current_text_locations.end() )
        {
            children_to_insert.emplace(std::move(current));
        }
        else
        {
            insert_as_child(result->second, child);
            current_text_locations.emplace_back(
                HashToNode {
                    (const char*) xmlGetProp(child, (const xmlChar*) "id"), child
                });
        }

        children_to_insert.pop();
    }

    xmlChar* output;
    int size;
    htmlDocDumpMemory(orihime_text_document, &output, &size);
    *request.rout << (char*) output;
    free(output);
    
    xmlFreeDoc(orihime_text_document);
}

void users_id_texts_id_GET_JSON(OrihimeRequest& request)
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

    rapidjson::OStreamWrapper wrapper {*request.rout};
    rapidjson::Writer<rapidjson::OStreamWrapper> writer {wrapper};
    root_text.Accept(writer);
} 

void users_id_texts_id_HEAD(OrihimeRequest& request) {} 
void users_id_texts_id_PUT(OrihimeRequest& request) {} 
void users_id_texts_id_POST(OrihimeRequest& request)
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
void users_id_words_GET(OrihimeRequest& request) {} 
void users_id_words_HEAD(OrihimeRequest& request) {} 
void users_id_words_POST(OrihimeRequest& request) {} 
void users_id_words_id_GET(OrihimeRequest& request) {} 
void users_id_words_id_HEAD(OrihimeRequest& request) {} 
void users_id_words_id_PUT(OrihimeRequest& request) {} 
