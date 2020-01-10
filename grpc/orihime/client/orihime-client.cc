#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "orihime.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using orihime::Source;
using orihime::TextHash;
using orihime::TextTreeNode;
using orihime::Orihime;

Source MakeSource(std::string name, int id)
{
    Source source;
    source.set_name(name);
    source.set_id(id);
    return source;
}

class OrihimeClient
{
public:
    OrihimeClient(std::shared_ptr<Channel> channel) : stub_(Orihime::NewStub(channel)) {};

    bool AddSource(const Source& request)
        {
            ClientContext context;
            Source response {};
            Status status = stub_->AddSource(&context, request, &response);

            if (!status.ok())
            {
                std::cout << "AddSource rpc failed." << std::endl;
                return false;
            }
            else
            {
                std::cout << "Got back a source with name " << response.name() << " and id " << response.id() << "\n";
            }

            return true;
        }

    Status AddText()
    {
        ClientContext context;
        orihime::Text request_text;
        orihime::Text response_text;
        Status status; 

        request_text.set_source_id(1);
        request_text.set_content("Hello there, here is a new text, thanks");
        status = stub_->AddText(&context, request_text, &response_text);

        std::cout << ( status.ok() ? "AddText Success\n" : "AddText rpc failed\n") ;
        return status;
    }

    Status TextTree()
    {
        uint8_t hash[] {0xc9, 0x7d, 0x03, 0xd4, 0xbd, 0x77, 0x62, 0x75, 0xf7, 0x11, 0xd1, 0xc9, 0x42, 0xc1, 0x57, 0xe2, 0x00};
        std::string user = "Test user";

        orihime::TextTreeQuery text_tree_query {};
        text_tree_query.set_hash(reinterpret_cast<char*>(&hash));
        text_tree_query.set_user(user);

        ClientContext context;
        std::shared_ptr<ClientReader<TextTreeNode>> stream(stub_->TextTree(&context, text_tree_query));

        TextTreeNode node;
        while ( stream->Read(&node) )
        {
            std::cout << "Got back the following TextTreeNode:\n"
                      << node.parent_hash() << "\n"
                      << node.definition_hash() << "\n"
                      << node.word() << "\n"
                      << node.definition() << "\n"
                      << node.source() << "\n";
        }

        return Status::OK;
    }

    std::unique_ptr<Orihime::Stub> stub_;
};

int main(int argc, char** argv) {

  OrihimeClient orihime(
      grpc::CreateChannel("localhost:50051",
                          grpc::InsecureChannelCredentials()));

  Source source {};
  source.set_name("Yo wassup");
  source.set_id(1);

  orihime.AddSource(source);
  orihime.AddText();
  orihime.TextTree();

  return 0;
}


