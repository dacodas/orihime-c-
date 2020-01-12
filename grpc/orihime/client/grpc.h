#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "orihime.grpc.pb.h"

class OrihimeClient
{
public:
    OrihimeClient() {};
    OrihimeClient(std::shared_ptr<grpc::Channel> channel) : stub_(orihime::Orihime::NewStub(channel)) {};

    grpc::Status AddSource(std::string& source_name);
    grpc::Status AddText(int source_id, std::string& text);
    grpc::Status AddChildWord(uint64_t user_id, std::vector<char>& parent_hash,
                              uint64_t definition_source_id, std::string& word, std::string& definition);
    grpc::Status TextTree(std::vector<uint8_t>& hash, std::string& user);
    grpc::Status TextTree(std::vector<char>& hash, std::string& user);
    grpc::Status TextTree(char* hash, std::string& user);

private:
    std::unique_ptr<orihime::Orihime::Stub> stub_;
};

extern OrihimeClient client;
