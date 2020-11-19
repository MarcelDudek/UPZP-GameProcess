#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include "client_communication.h"
#include <asio.hpp>
#include <iostream>

int main() {
  try {
    asio::io_context context;
    upzp::client_com::ClientCommunication client_comm(context, 2137);
    client_comm.Start();
    context.run();  
  } catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }

  while (true);
}
