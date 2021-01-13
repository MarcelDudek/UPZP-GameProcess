#pragma once

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

namespace upzp::mainproc_com {

/**
 * @brief Main process in TCP communication.
*/
    class MainProcTcp{
    private:

    public:
        MainProcTcp();
        int Start();
    };

}