#pragma once

#include "client.h"
#include <asio.hpp>

namespace upzp::client_com {

/**
 * @brief Client in UDP communication.
*/
class ClientUdp : Client {
 public:
  asio::ip::udp::endpoint remote_endpoint_;  /**< Client remote endpoint for communication. */
  bool DecodeDatagram(const char* buffer, const size_t size);
};

}  // namespace upzp:client_com