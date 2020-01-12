#include <string>
#include <unordered_map>
#include <functional>

#include "grpc.h"
#include "add.h"

// Look into `git`s `/usr/libexec` and `github.com/git/git/git.c` as an alternative
// 
// struct command_struct commands[] = {
//     { "add", cmd_add }
// };

typedef std::unordered_map<std::string, std::function<void(int, char**)>> subcommand_dictionary_value_t;
typedef std::unordered_map<std::string, subcommand_dictionary_value_t> subcommand_dictionary_t;
subcommand_dictionary_t subcommand_dictionary
{
    {
        "add",
        {
            {"source", AddSource}, 
            {"text", AddText},
            {"child-word", AddChildWord}
        }
    }
};

int main(int argc, char** argv)
{
    client = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

    subcommand_dictionary_t::iterator subcommand_match = subcommand_dictionary.find(argv[1]);
    if ( subcommand_match == subcommand_dictionary.end() )
    {
        std::cerr << "Didn't find subcommand '" << argv[1] << "'\n";
        exit(1);
    }

    subcommand_dictionary_value_t subsubcommand_dictionary = subcommand_match->second;
    subcommand_dictionary_value_t::iterator subsubcommand_match = subsubcommand_dictionary.find(argv[2]);
    if ( subsubcommand_match == subsubcommand_dictionary.end() )
    {
        std::cerr << "Didn't find valid '" << argv[2] << "' for subcommand '" << argv[1] << "'\n";
        exit(1);
    }

    subsubcommand_match->second(argc, argv);

    return 0;
}
