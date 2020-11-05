#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include <iostream>
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include "test_data_generated.h"

int main() {
  asio::error_code err;
  asio::io_service service;
  asio::io_context context;

  int port;
  int buffer_size;
  std::cout << "Podaj port: ";
  std::cin >> port;
  std::cout << "Podaj rozmiar buforu w bajtach: ";
  std::cin >> buffer_size;

  asio::ip::tcp::acceptor acceptor(service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
  asio::ip::tcp::socket socket(service);

  acceptor.accept(socket);

  std::vector<char> buf(buffer_size);
  // asio::read_until(socket, buf, "\n", err);
  asio::read(socket, asio::buffer(buf.data(), buf.size()));
  if (err) {
    std::cout << err.message() << "\t" << err.value() << std::endl;
  }
  
  std::cout << "Serialized data:\t";
  for (auto c : buf) std::cout << c;
  std::cout << std::endl;
  
  auto test_data = TestData::GetTester(buf.data());
  std::cout << "Decoded data:\n\n"
               "Pos: {"
            << test_data->pos()->x() << ", " << test_data->pos()->y() << ", "
            << test_data->pos()->z()
            << "}\nSome integer: "
            << test_data->some_integer()
            << "\nSome string: " << test_data->some_string()->c_str() << std::endl;

  return 0;
}