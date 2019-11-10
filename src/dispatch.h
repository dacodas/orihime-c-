#pragma once 

#include <vector>
#include <string>
#include <regex>
#include <tuple>

#include <fcgio.h>

#include "OrihimeRequest.h"
#include "api.h"

using Function = std::function<void(OrihimeRequest&)>;
using HTTPMethodToFunction = std::vector<std::pair<std::string, Function>>;
using Dispatch = std::pair<std::regex, HTTPMethodToFunction>;

// Should we also do the URL matching in httpd and then set the
// variable with the function name?

static std::vector<Dispatch> DispatchTable
    {};

void dispatch(OrihimeRequest& request)
{
    std::smatch match;
    std::string path {request.parameter("REQUEST_URI")};
    std::string method {request.parameter("REQUEST_METHOD")};

    for ( const Dispatch& dispatch : DispatchTable )
    {
        auto& [regex, http_method_to_function] = dispatch;

        if ( std::regex_match(path, match, regex) )
        {
            auto result = std::find_if(
                http_method_to_function.begin(),
                http_method_to_function.end(),
                [&] (const std::pair<std::string, Function>& a) { return a.first == method; }
                );

            if ( result != http_method_to_function.end() )
            {
                result->second(request);
            }

            break;
        }
    }
}
