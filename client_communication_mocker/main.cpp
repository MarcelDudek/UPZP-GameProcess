#include <iostream>
#include <chrono>
#include <thread>
#include <asio.hpp>
#include "inc/udp_communication_mocker.h"

asio::ip::udp::endpoint remote_endpoint_g;

void Receive(asio::ip::udp::socket& socket, std::vector<char>& buffer) {
  socket.async_receive_from(
      asio::buffer(buffer.data(), buffer.size()), remote_endpoint_g,
      [&socket, &buffer](const asio::error_code& error,
                        std::size_t bytes_transfered) {
        std::cout << "Received " << bytes_transfered << " bytes of data from " << remote_endpoint_g.address().to_string()
          << ":" << remote_endpoint_g.port() << std::endl;
        std::cout << upzp::mocker::DecodeGameStatus(buffer.data(), bytes_transfered) << std::endl;
        using namespace std::chrono_literals;
        Receive(socket, buffer);
      });
}

void Transmit(asio::ip::udp::socket& socket, asio::high_resolution_timer& timer) {
  using namespace std::chrono_literals;
  timer.expires_after(2000ms);

  timer.async_wait([&socket, &timer](const asio::error_code& error) {
    static size_t seq = 0;
    auto datagram = upzp::mocker::MockInputDatagram(++seq, 1, true, 3.14);
    auto buffer = datagram.Get();

    asio::ip::udp::endpoint remote_endpoint = asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 8850);
    socket.async_send_to(asio::buffer(buffer.data(), buffer.size()), remote_endpoint, [&remote_endpoint, &socket, &buffer](const asio::error_code& error,
      std::size_t bytes_transfered) {
        std::cout << "Transmited " << bytes_transfered << " bytes of data.\n";
      });
    Transmit(socket, timer);
    });
}

int main() {
  asio::io_context context;
  std::vector<char> buffer(4096);
  asio::ip::udp::socket socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 4000));
  asio::high_resolution_timer timer(context);

  Receive(socket, buffer);
  Transmit(socket, timer);
  context.run();
}