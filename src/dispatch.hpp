#include <vector>
#include <string>
#include <regex>
#include <tuple>

#include <fcgio.h>

#include "api.hpp"

using Parameters = std::vector<std::string>;
using SQLQueryFunction = std::function<void(const std::vector<std::string>&)>;
using HTTPMethodToSQLQueryFunction = std::vector<std::pair<std::string, SQLQueryFunction>>;
using Dispatch = std::pair<std::regex, HTTPMethodToSQLQueryFunction>;
// using DispatchTable = std::vector<Dispatch>;

// Why am I doing this, I thought this is what Apache was for?
// using DispatchFunction = std::pair<std::regex, DispatchFunctions>;
// static std::vector<DispatchFunction> DispatchTable;

static std::vector<Dispatch> DispatchTable
    {
        Dispatch("/sources/", {
                {"GET", sources_GET},
                {"HEAD", sources_HEAD},
                {"POST", sources_POST}
            }),

        Dispatch("/sources/(.*?)", {
                {"GET", sources_id_GET},
                {"HEAD", sources_id_HEAD},
            }),
        
        Dispatch("/texts/", {
                {"GET", texts_GET},
                {"HEAD", texts_HEAD},
            }),

        Dispatch("/texts/(.*?)", {
                {"GET", texts_id_GET},
                {"HEAD", texts_id_HEAD},
            }),

        Dispatch("/words/", {
                {"GET", words_GET},
                {"HEAD", words_HEAD}
            }),

        Dispatch("/words/(.*?)", {
                {"GET", words_id_GET},
                {"HEAD", words_id_HEAD}
            }),

        Dispatch("/users/(.*?)/texts/", {
                {"GET", users_id_texts_GET},
                {"HEAD", users_id_texts_HEAD},
                {"POST", users_id_texts_POST}
            }),

        Dispatch("/users/(.*?)/texts/(.*?)", {
                {"GET", users_id_texts_id_GET},
                {"HEAD", users_id_texts_id_HEAD},
                {"PUT", users_id_texts_id_PUT}
            }),

        Dispatch("/users/(.*?)/words/", {
                {"GET", users_id_words_GET},
                {"HEAD", users_id_words_HEAD},
                {"POST", users_id_words_POST}
            }),

        Dispatch("/users/(.*?)/words/(.*?)", {
                {"GET", users_id_words_id_GET},
                {"HEAD", users_id_words_id_HEAD},
                {"PUT", users_id_words_id_PUT}
            })
    };

void dispatch(FCGX_Request&& request)
{

}
