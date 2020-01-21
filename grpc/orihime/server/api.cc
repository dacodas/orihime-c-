#include <queue>
#include <tuple>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include <openssl/md5.h>

#include "sql.h"
#include "orihime.grpc.pb.h"

namespace orihime {

    void AddText(uint64_t source_id, const std::string& contents)
    {
        std::unique_ptr<sql::PreparedStatement> statement {connection->prepareStatement(
                R"sql(
INSERT INTO text (source, contents, hash) VALUES (?, ?, ?);
)sql")};

        unsigned char hash[MD5_DIGEST_LENGTH];
        MD5((unsigned char*) contents.data(), contents.size(), hash);
        std::string hash_as_string {reinterpret_cast<char*>(&hash), MD5_DIGEST_LENGTH};

        statement->setUInt64(1, source_id);
        statement->setString(2, contents);
        statement->setString(3, hash_as_string);

        

        std::shared_ptr<sql::ResultSet> result {statement->executeQuery()};
    }

    void TextTree(const std::string& user, const std::string& hash, grpc::ServerWriter<orihime::TextTreeNode>* stream)
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

    void AddChildWord(std::string& word, std::string& definition, uint64_t definition_source_id, uint64_t user_id, char* hash)
    {
        std::unique_ptr<sql::PreparedStatement> text_insert_statement {connection->prepareStatement(
                R"sql(
INSERT INTO word (word, definition) VALUES (?, definition.id)
SELECT id AS definition.id FROM text WHERE hash = ?;
)sql")};


        AddText(definition_source_id, definition);


// INSERT INTO word_relation (user, text, word) VALUES (?, text.id, word.id)
// SELECT text.id, word.id
// FROM word 
// INNER JOIN text ON word.definition = text.id
// WHERE word.word = ?;
    }
}
