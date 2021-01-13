#pragma once

#include "mainproc_tcp.h"
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
 * @brief Main process communication main class.
*/
    class MainProcCommunication {
    private:

    public:
        MainProcCommunication();
        int Start();
    };

}