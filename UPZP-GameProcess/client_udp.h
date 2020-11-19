#pragma once

#include "client.h"
#include <asio.hpp>

namespace upzp::client_com {

/**
 * @brief Client in UDP communication.
*/
struct ClientUdp : Client {
  asio::ip::udp::endpoint remote_endpoint_;  /**< Client remote endpoint for communication. */
};

}  // namespace upzp:client_com