#pragma once

#include "client_udp.h"
#include <asio.hpp>
#include <vector>

namespace upzp::client_com {

/**
 * @brief Client communication main class.
*/
class ClientCommunication {
 private:
  asio::ip::udp::socket socket_;
  asio::ip::udp::endpoint remote_endpoint_;
  std::vector<ClientUdp> clients_;
  std::vector<char> buffer_;

 public:
  explicit ClientCommunication(asio::io_context& context, const unsigned int port);
  void Start();
};

}  // namespace upzp::client_com
