#pragma once

#include "client_udp.h"
#include <asio.hpp>
#include <vector>
#include <thread>

namespace upzp::client_com {

/**
 * @brief Client communication main class.
*/
class ClientCommunication {
 private:
  static constexpr std::size_t RECEIVE_BUFFER_SIZE = 4096;

  asio::ip::udp::socket socket_;
  asio::ip::udp::endpoint remote_endpoint_;
  std::vector<ClientUdp> clients_;
  std::vector<char> receive_buffer_;
  std::vector<char> transmit_buffer_;
  std::thread transmit_thread_;

  void StartReceive();
  void StartTransmit();

 public:
  ClientCommunication(asio::io_context& context, const unsigned int port);
  void AddClient(Client client);
  void AddClient(std::vector<Client>& client);
  void Start();
};

}  // namespace upzp::client_com
