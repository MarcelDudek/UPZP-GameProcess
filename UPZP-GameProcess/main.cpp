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

  asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", err), 2137);
  asio::ip::tcp::socket socket(context);
  socket.connect(endpoint, err);
  if (!err) {
    std::cout << "Connected!\n";
  } else {
    std::cout << "Failed to connect to address:\n"
              << err.message() << std::endl;
  }

  if (socket.is_open()) {
    std::string request = "Testowe wyslanie danych\n";

    socket.write_some(asio::buffer(request.data(), request.size()), err);
  }

  return 0; 
}