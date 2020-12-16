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
        using namespace std::chrono_literals;
        Receive(socket, buffer);
      });
}

void Transmit(asio::ip::udp::socket& socket, std::vector<char>& buffer, asio::high_resolution_timer& timer) {
  using namespace std::chrono_literals;
  timer.expires_after(2000ms);

  timer.async_wait([&socket, &buffer, &timer](const asio::error_code& error) {
    asio::ip::udp::endpoint remote_endpoint = asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 2137);
    socket.async_send_to(asio::buffer(buffer.data(), buffer.size()), remote_endpoint, [&remote_endpoint, &socket, &buffer](const asio::error_code& error,
      std::size_t bytes_transfered) {
        std::cout << "Transmited " << bytes_transfered << " bytes of data.\n";
      });
    Transmit(socket, buffer, timer);
    });
}

int main() {
  asio::io_context context;
  std::vector<char> buffer(4096);
  asio::ip::udp::socket socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 8585));
  asio::high_resolution_timer timer(context);

  static size_t seq = 0;
  auto datagram = upzp::mocker::MockInputDatagram(++seq, 0x1234, true, 3.14);
  auto data = datagram.Get();

  Receive(socket, buffer);
  Transmit(socket, data, timer);
  context.run();
}