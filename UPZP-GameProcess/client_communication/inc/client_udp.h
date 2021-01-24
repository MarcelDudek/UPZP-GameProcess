#pragma once

#include "../../inc/client.h"
#include "../../inc/player_input.h"
#include "../../inc/player_input_generated.h"
#include <asio.hpp>

namespace upzp::client_com {

/**
 * @brief Client in UDP communication.
*/
class ClientUdp : public Client {
 private:
  static constexpr int DATAGRAM_PLAYER_INPUT_VER = 101;

  PlayerInput input_;
  uint64_t last_sequence_;

 public:
  ClientUdp(const std::string& name, uint32_t id, VehicleType vehicle, const std::string& ip,
            unsigned int port);
  explicit ClientUdp(Client& client);

  const asio::ip::udp::endpoint remote_endpoint_;  /**< Client remote endpoint for communication. */
  bool DecodeDatagram(const char* buffer, size_t size);
  PlayerInput Input();
};

}  // namespace upzp:client_com