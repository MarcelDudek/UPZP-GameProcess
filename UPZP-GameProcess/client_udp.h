#pragma once

#include "client.h"
#include "player_input.h"
#include "player_input_generated.h"
#include <asio.hpp>

namespace upzp::client_com {

/**
 * @brief Client in UDP communication.
*/
class ClientUdp : Client {
 private:
  static constexpr int DATAGRAM_PLAYER_INPUT_VER = 1;

  const Upzp::PlayerInput::Input* player_input_;

 public:
  ClientUdp(const std::string name, const uint32_t id, const VehicleType vehicle);
  ~ClientUdp();

  asio::ip::udp::endpoint remote_endpoint_;  /**< Client remote endpoint for communication. */
  bool DecodeDatagram(const char* buffer, const size_t size);
  PlayerInput Input();
};

}  // namespace upzp:client_com