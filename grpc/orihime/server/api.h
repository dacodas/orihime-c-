#pragma once

void TextTreeSQL(const std::string& user, const std::string& hash, grpc::ServerWriter<orihime::TextTreeNode>* stream);
