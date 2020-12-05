#include <iostream>
#include <chrono>
#include <thread>
#include <asio.hpp>
#include "udp_communication_mocker.h"

void Receive(asio::ip::udp::socket& socket, std::vector<char>& buffer) {
  asio::ip::udp::endpoint remote_endpoint;
  socket.async_receive_from(
      asio::buffer(buffer.data(), buffer.size()), remote_endpoint,
      [&remote_endpoint, &socket, &buffer](const asio::error_code& error,
                        std::size_t bytes_transfered) {
        if (remote_endpoint.address().to_string() == "127.0.0.1" &&
            remote_endpoint.port() == 2137) {
          std::cout << "Received " << bytes_transfered << " bytes of data.\n";
        }
        Receive(socket, buffer);
      });
}

int main() {
  asio::io_context context;
  std::vector<char> buffer(4096);
  asio::ip::udp::socket socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 8585));
  Receive(socket, buffer);

  while (true) {
    static size_t seq = 0;
    auto datagram = upzp::mocker::MockInputDatagram(++seq, 0x1234, true, 3.14);
    auto data = datagram.Get();
    socket.send_to(asio::buffer(data.data(), data.size()), asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 2137));
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(500ms);
  }
}