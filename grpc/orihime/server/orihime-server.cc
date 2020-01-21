#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "orihime.grpc.pb.h"

#include "api.h"
#include "sql.h"

class OrihimeImpl final : public orihime::Orihime::Service
{
public:
    explicit OrihimeImpl() {}

    grpc::Status AddSource(grpc::ServerContext* context,
                           const orihime::Source* request,
                           orihime::Source* response) override
    {
        response->set_name("Some source");
        response->set_id(1);
        return grpc::Status::OK;
    }

    grpc::Status AddText(grpc::ServerContext* context,
                         const orihime::Text* request_text,
                         orihime::Text* response_text) override
    {
        orihime::AddText(request_text->source_id(), request_text->content());

        *response_text = *request_text;
        return grpc::Status::OK;
    }

                         

    grpc::Status TextTree(grpc::ServerContext* context,
                          const orihime::TextTreeQuery* request,
                          grpc::ServerWriter<orihime::TextTreeNode>* stream) override
    {
        std::string hash = request->hash();
        std::string user = request->user();
        orihime::TextTree(user, hash, stream);

        return grpc::Status::OK;
    }

    grpc::Status AddChildWord(grpc::ServerContext* context,
                              const orihime::ChildWord* request_child_word,
                              orihime::Word* response_word) override
    {
        
    }
};

void RunServer()
{
    std::string server_address("0.0.0.0:50051");
    OrihimeImpl service {};

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv)
{
    orihime_sql();
    RunServer();

    return 0;
}
