#include <asio.hpp>
#include <iostream>
#include <vector>
#include "tcp_communication_mocker.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

int main () {
  using asio::ip::tcp;
  asio::io_context context;
  tcp::acceptor acceptor(context, tcp::endpoint(tcp::v4(), 3000));
  std::vector<char> receive_buffer(1024);

  while (true) {
    tcp::socket socket(context);
    acceptor.accept(socket);
    asio::error_code error;
    auto length = socket.read_some(asio::buffer(receive_buffer), error);
    std::cout << "Received " << length << " bytes of data." << std::endl;
    auto message = upzp::mocker::MockPlayerListDatagram(5, 5);
    auto buffer = message.Get();
    buffer.insert(buffer.begin(), {'t', 'e', 's', 't'});
    buffer.insert(buffer.end(), {'t', 'e', 's', 't', static_cast<char>(0xAB)});
    asio::write(socket, asio::buffer(buffer), error);
    length = socket.read_some(asio::buffer(receive_buffer), error);
    std::cout << "Received " << length << " bytes of data." << std::endl;
  }
}
#pragma clang diagnostic pop