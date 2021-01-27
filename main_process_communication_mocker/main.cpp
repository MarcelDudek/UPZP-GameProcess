#include <asio.hpp>
#include "tcp_communication_mocker.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

int main () {
  using asio::ip::tcp;
  asio::io_context context;
  tcp::acceptor acceptor(context, tcp::endpoint(tcp::v4(), 3000));

  while (true) {
    tcp::socket socket(context);
    acceptor.accept(socket);
    auto message = upzp::mocker::MockPlayerListDatagram(5, 5);
    auto buffer = message.Get();
    buffer.insert(buffer.begin(), {'t', 'e', 's', 't'});
    buffer.insert(buffer.end(), {'t', 'e', 's', 't', static_cast<char>(0xAB)});
    asio::error_code error;
    asio::write(socket, asio::buffer(buffer), error);
    asio::write(socket, asio::buffer(buffer), error);
  }
}
#pragma clang diagnostic pop