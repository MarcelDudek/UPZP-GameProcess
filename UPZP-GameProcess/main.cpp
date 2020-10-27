#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include <iostream>
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

int main() { 
  asio::error_code err;
  asio::io_context context;

  asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", err), 80);
  asio::ip::tcp::socket socket(context);
  socket.connect(endpoint, err);
  if (!err) {
    std::cout << "Connected!\n";
  } else {
    std::cout << "Failed to connect to address:\n"
              << err.message() << std::endl;
  }

  return 0; 
}