#include <queue>
#include <tuple>

#include "sql.h"
#include "orihime.grpc.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

void TextTreeSQL(const std::string& user, const std::string& hash, grpc::ServerWriter<orihime::TextTreeNode>* stream)
{
    std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement(
            R"sql(
CALL orihime.TextTree(?, ?);
)sql")};

    statement->setString(1, user);
    statement->setString(2, hash);
    std::shared_ptr<sql::ResultSet> result {statement->executeQuery()};

    while ( result->next() )
    {
        orihime::TextTreeNode node;
        node.set_parent_hash({0, 0, 0, 0});
        node.set_definition_hash({0, 0, 0, 0});
        node.set_word(result->getString("word"));
        node.set_definition(result->getString("definition"));
        node.set_source(result->getString("source"));

        stream->Write(node);
    }

    // https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-tutorials-stored-routines-statement-with-result.html
    while ( statement->getMoreResults() ) {}
}

