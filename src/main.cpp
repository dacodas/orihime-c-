#include <thread>

#include <fcgio.h>

#include "config.h"

int main(int argc, char* argv[])
{
    FCGX_Init();

    std::thread work(per_thread);
    work.join();

    return 0;
}
