#pragma once 

#include <unordered_map>
#include <string>
#include <functional>

#include <fcgio.h>

#include "OrihimeRequest.h"
#include "api.h"

// Generated, do not edit
static std::unordered_map<std::string, std::function<void(OrihimeRequest&)>> dispatch_table {
    {"sources_GET", sources_GET},
    {"sources_HEAD", sources_HEAD},
    {"sources_POST", sources_POST},
    {"sources_id_GET", sources_id_GET},
    {"sources_id_HEAD", sources_id_HEAD},
    {"texts_GET", texts_GET},
    {"texts_HEAD", texts_HEAD},
    {"texts_id_GET", texts_id_GET},
    {"texts_id_HEAD", texts_id_HEAD},
    {"words_GET", words_GET},
    {"words_HEAD", words_HEAD},
    {"words_id_GET", words_id_GET},
    {"words_id_HEAD", words_id_HEAD},
    {"users_id_texts_GET", users_id_texts_GET},
    {"users_id_texts_HEAD", users_id_texts_HEAD},
    {"users_id_texts_POST", users_id_texts_POST},
    {"users_id_texts_id_GET", users_id_texts_id_GET},
    {"users_id_texts_id_HEAD", users_id_texts_id_HEAD},
    {"users_id_texts_id_PUT", users_id_texts_id_PUT},
    {"users_id_texts_id_POST", users_id_texts_id_POST},
    {"users_id_words_GET", users_id_words_GET},
    {"users_id_words_HEAD", users_id_words_HEAD},
    {"users_id_words_POST", users_id_words_POST},
    {"users_id_words_id_GET", users_id_words_id_GET},
    {"users_id_words_id_HEAD", users_id_words_id_HEAD},
    {"users_id_words_id_PUT", users_id_words_id_PUT}
};

void dispatch(OrihimeRequest& request)
{
    std::string function {request.parameter("FUNCTION")};

    auto result = dispatch_table.find(function);

    if ( result != dispatch_table.end() )
    {
        result->second(request);
    }
    else
    {
        *request.rout << "Function '" << function.c_str() << "' not found\n";
    }

    // for ( char **env = request.envp; *env != 0; ++env )
    //     *request.rout << *env << "\n";
}
