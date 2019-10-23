#include <fcgiapp.h>

#include "OrihimeRequest.hpp"
#include "dispatch.hpp"

#ifdef FAUX_FCGI
void mainLoop(FCGX_Request& request)
{
    if ( true )
    {
        OrihimeRequest request {environ};

        dispatch(std::move(request));
    }
}
#else
void mainLoop(FCGX_Request& request)
{
    while ( FCGX_Accept_r(&request) == 0 )
    {
        OrihimeRequest request {request};

        dispatch(std::move(request));
    }
}
#endif
