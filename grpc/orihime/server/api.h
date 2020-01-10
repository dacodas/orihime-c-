#pragma once

namespace orihime {

    void AddText(uint64_t source_id, const std::string& contents);
    void TextTree(const std::string& user, const std::string& hash, grpc::ServerWriter<orihime::TextTreeNode>* stream);

}
