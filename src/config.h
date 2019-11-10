#pragma once 

#include <mutex>

#include <fcgiapp.h>

#include "OrihimeRequest.h"
#include "dispatch.h"
#include "sql.h"

#ifdef FAUX_FCGI
void per_thread()
{
    orihime_sql();
    OrihimeRequest request(environ);

    *request.rout <<
        "Content-type: text/html\r\n"
        "\r\n";

    dispatch(request);

    mysql_thread_end();
}
#else
void per_thread()
{
    std::cout << "Starting sql connection\n";

    orihime_sql();
    OrihimeRequest request {};


    // From fcgi2 threaded.c
    for (;;)
    {
        // Shared mutex between all threads, so only one can accept a
        // connection at a time
        // 
        // I wonder if I need this mutex since the OS_Accept function
        // that is called acquires a lock on the socket
        // 
        // https://github.com/FastCGI-Archives/fcgi2/blob/cb44d8904e8db643f1e6ec7a48909c201df3df52/libfcgi/os_unix.c#L1159
        //
        // I think I don't need it, I think they just have it for the
        // Windows library... I will check in a bit
        static std::mutex accept_mutex;

        int accept_result;
        {
            std::scoped_lock lock(accept_mutex);
            accept_result = FCGX_Accept_r(&request);
        }

        if ( accept_result < 0 )
        {
            std::cerr << "Could not accept request\n";
            break;
        }

        request.prepare_streams();

        *request.rout <<
            "Content-type: text/html\r\n"
            "\r\n"
            ;

        dispatch(request);
    }
    
    mysql_thread_end();
}
#endif
