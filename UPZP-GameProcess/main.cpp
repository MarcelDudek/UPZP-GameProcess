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
  asio::io_context context;

  std::string ip_address;
  int port;
  std::cout << "Podaj adres IP: ";  // "127.0.0.1"
  std::cin >> ip_address;
  std::cout << "Podaj port: ";
  std::cin >> port;

  asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip_address, err), port);
  asio::ip::tcp::socket socket(context);
  socket.connect(endpoint, err);
  if (!err) {
    std::cout << "Connected!\n";
  } else {
    std::cout << "Failed to connect to address:\n"
              << err.message() << std::endl;
  }
  flatbuffers::FlatBufferBuilder builder(1024);
  auto flat_string = builder.CreateString("Testowy String!");
  TestData::Vec3 vec3{1.1, 2.2, 3.3};

  auto tester = TestData::CreateTester(builder, &vec3, 123, flat_string);
  builder.Finish(tester);

  if (socket.is_open()) {
    std::string request = "Koniec\n";

    std::cout << "SIZE: " << builder.GetSize() << std::endl;

    socket.write_some(asio::buffer(builder.GetBufferPointer(), builder.GetSize()), err);
    //socket.write_some(asio::buffer(request.data(), request.size()), err);
  }

  return 0; 
}