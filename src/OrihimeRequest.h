#pragma once 

#include <iostream>

#include <fcgiapp.h>
#include <fcgio.h>

// This class should set up the necessary infrastructure to run a
// request standalone or as part of an FCGI server

class OrihimeRequest : public FCGX_Request
{
private:
    fcgi_streambuf rin_streambuf;
    fcgi_streambuf rout_streambuf;
    fcgi_streambuf rerr_streambuf;

    // We don't necessarily own these, as they can point to
    // std::{cin,cout,cerr}
    //
    // These need to be pointers/references and the default
    // constructor for these is deleted
public:

    std::istream* rin;
    std::ostream* rout;
    std::ostream* rerr;

    OrihimeRequest(char** environ)
        {
            envp = environ;
            rin = &std::cin;
            rout = &std::cout;
            rerr = &std::cerr;
        }

    OrihimeRequest()
        : FCGX_Request()
        {
            FCGX_InitRequest(this, 0, 0);
        };

    OrihimeRequest(const OrihimeRequest&) = delete;
    OrihimeRequest& operator=(const OrihimeRequest&) = delete;
    OrihimeRequest(OrihimeRequest&&) = delete;
    OrihimeRequest& operator=(OrihimeRequest&&) = delete;

    ~OrihimeRequest()
        {
            if ( rin == &std::cin )
                return;
            else
            {
                delete(rin);
                delete(rout);
                delete(rerr);
            }

            FCGX_Finish_r(this);
        }

    // This is called after a request has been accepted
    void prepare_streams()
        {
            rin_streambuf = fcgi_streambuf {this->in};
            rout_streambuf = fcgi_streambuf {this->out};
            rerr_streambuf = fcgi_streambuf {this->err};

            rin = new std::istream(&rin_streambuf);
            rout = new std::ostream(&rout_streambuf);
            rerr = new std::ostream(&rerr_streambuf);
        }

    std::string parameter(const std::string& parameter) const
        {
            char* parameter_c = FCGX_GetParam(parameter.c_str(), envp);
            return parameter_c ? parameter_c : "";
        }
};
