#pragma once 

#include <iostream>

#include <fcgiapp.h>

thread_local std::istream rin {std::cin.rdbuf()};
thread_local std::ostream rout {std::cout.rdbuf()};
thread_local std::ostream rerr {std::cerr.rdbuf()};

class OrihimeRequest : public FCGX_Request
{
private:
    fcgi_streambuf rin_streambuf;
    fcgi_streambuf rout_streambuf;
    fcgi_streambuf rerr_streambuf;
    
public:

    OrihimeRequest(char** environ)
        {
            envp = environ;
        }
        
    OrihimeRequest(FCGX_Request& request) :
        FCGX_Request {request}, 
        rin_streambuf {request.in}, 
        rout_streambuf {request.out}, 
        rerr_streambuf {request.err}
        {
            rin.rdbuf(&rin_streambuf);
            rout.rdbuf(&rout_streambuf);
            rerr.rdbuf(&rerr_streambuf);
        };

    std::string parameter(const std::string& parameter)
        {
            char* parameter_c = FCGX_GetParam(parameter.c_str(), envp);
            return parameter_c ? parameter_c : "";
        }
};
