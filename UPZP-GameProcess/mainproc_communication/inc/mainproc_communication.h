#pragma once

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>
#include <asio.hpp>
#include <mutex>

namespace upzp::mainproc_com {

/**
 * @brief Main process communication main class.
*/
    class MainProcCommunication {
    private:
        void StartReceive();
        void StartTransmit();
        void GenerateInfo();
        unsigned int port;
        std::string ipAddress;
        asio::io_context context_;
        asio::ip::tcp::socket socket_;
        asio::ip::tcp::endpoint remote_endpoint_;

        static constexpr std::size_t RECEIVE_BUFFER_SIZE = 4096;
        std::vector<char> receive_buffer_;
        std::vector<char> transmit_buffer_;

        std::thread run_thread_;
        bool running_;
    public:
        MainProcCommunication(unsigned int port, std::string ipAddress);
        void Start();
    };

}