#include <cppconn/resultset.h>

#include <rapidjson/document.h>
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

void output_array_from_sql(std::unique_ptr<sql::ResultSet>& result)
{
    rapidjson::Document document {};
    document.SetArray();

    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    while ( result->next() )
    {
        sql::SQLString string {result->getString("name")};

        // I really tried to avoid this copy
        rapidjson::Value value;
        value.SetString(string.c_str(), string.asStdString().size(), allocator);

        document.PushBack(value.Move(), allocator);
    }

    rapidjson::OStreamWrapper wrapper {std::cout};
    rapidjson::Writer<rapidjson::OStreamWrapper> writer {wrapper};
    document.Accept(writer);
}
