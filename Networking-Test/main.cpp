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
  asio::io_service service;
  asio::io_context context;

  asio::ip::tcp::acceptor acceptor(service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 2137));
  asio::ip::tcp::socket socket(service);

  acceptor.accept(socket);

  asio::streambuf buf;
  asio::read_until(socket, buf, "\n", err);
  if (err) {
    std::cout << err.message() << "\t" << err.value() << std::endl;
  }
  std::string data = asio::buffer_cast<const char*>(buf.data());
  std::cout << data;

  system("pause");

  return 0;
}