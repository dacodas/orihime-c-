#include "grpc.h"

OrihimeClient client {};

grpc::Status OrihimeClient::AddSource(std::string& source_name)
{
    grpc::ClientContext context;
    orihime::Source request_source;
    orihime::Source response_source;
    grpc::Status status;

    request_source.set_name(source_name);
    status = stub_->AddSource(&context, request_source, &response_source);

    if ( !status.ok() ) {
        std::cerr << "AddSource failed: (" << status.error_code() << ") " << status.error_message() << "\n";
        exit(1);
    }

    return status;
}

grpc::Status OrihimeClient::AddText(int source_id, std::string& text)
{
    grpc::ClientContext context;
    orihime::Text request_text;
    orihime::Text response_text;
    grpc::Status status; 

    request_text.set_source_id(source_id);
    request_text.set_content(text);
    status = stub_->AddText(&context, request_text, &response_text);

    if ( !status.ok() ) {
        std::cerr << "AddText failed: (" << status.error_code() << ") " << status.error_message() << "\n";
        exit(1);
    }

    return status;
}

grpc::Status OrihimeClient::AddChildWord(uint64_t user_id, std::vector<char>& parent_hash,
                                         uint64_t definition_source_id, std::string& word, std::string& definition)
{
    grpc::ClientContext context;
    orihime::ChildWord request_child_word;
    orihime::Word response_word;
    grpc::Status status; 

    request_child_word.set_word(word);
    request_child_word.set_definition(definition);
    request_child_word.set_definition_source_id(definition_source_id);
    request_child_word.set_parent_hash(reinterpret_cast<char*>(parent_hash.data()));
    request_child_word.set_user_id(user_id);
    status = stub_->AddChildWord(&context, request_child_word, &response_word);

    if ( !status.ok() ) {
        std::cerr << "AddChildWord failed: (" << status.error_code() << ") " << status.error_message() << "\n";
        exit(1);
    }

    return status;
}

grpc::Status OrihimeClient::TextTree(std::vector<uint8_t>& hash, std::string& user)
{
    return TextTree(reinterpret_cast<char*>(hash.data()), user);
}

grpc::Status OrihimeClient::TextTree(std::vector<char>& hash, std::string& user)
{
    return TextTree(reinterpret_cast<char*>(hash.data()), user);
}

grpc::Status OrihimeClient::TextTree(char* hash, std::string& user)
{
    orihime::TextTreeQuery text_tree_query {};
    text_tree_query.set_hash(hash);
    text_tree_query.set_user(user);

    grpc::ClientContext context;
    std::shared_ptr<grpc::ClientReader<orihime::TextTreeNode>> stream(stub_->TextTree(&context, text_tree_query));

    orihime::TextTreeNode node;
    while ( stream->Read(&node) )
    {
        std::cout << "Got back the following TextTreeNode:\n"
                  << node.parent_hash() << "\n"
                  << node.definition_hash() << "\n"
                  << node.word() << "\n"
                  << node.definition() << "\n"
                  << node.source() << "\n";
    }

    return grpc::Status::OK;
}
