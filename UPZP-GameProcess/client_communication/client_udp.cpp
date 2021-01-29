#include "inc/client_udp.h"
#include "../datagram/inc/datagram.h"

namespace upzp::client_com {

/**
 * @brief Constructor.
 * @param name Client's name.
 * @param id Client's ID.
 * @param vehicle Client's vehicle type.
 * @param ip Client's IP v4.
*/
ClientUdp::ClientUdp(const std::string& name, const uint32_t id,
  const VehicleType vehicle, const std::string& ip, const unsigned int port)
  : Client(name, id, vehicle, ip, port), remote_endpoint_(asio::ip::make_address(ip), port) {
  input_.id = this->id_;
  input_.move = false;
  input_.direction = 0.0f;

  last_sequence_ = 0;
}

/**
 * @brief Contrctor.
 * @param client Client.
*/
ClientUdp::ClientUdp(Client& client) : ClientUdp(client.Name(), client.Id(), client.Vehicle(), client.Ip_v4(), client.Port()) {}

/**
 * @brief Decode received datagram.
 * @param buffer Datagram's buffer.
 * @param size Buffer's size.
 * @return True for succesful decode.
 * If seq num is lower than current
 * false is also returned.
*/
bool ClientUdp::DecodeDatagram(const char* buffer, const size_t size) {
  Datagram datagram;
  datagram.Load(buffer, size);

  if (datagram.Version() == DATAGRAM_PLAYER_INPUT_VER) {
    auto payload = datagram.Payload();
    auto input = Upzp::PlayerInput::GetInput(payload.data());
    if (input->sequence() > last_sequence_ && input->id() == id_) {  // if sequence is bigger (newer input)
      input_.move = input->move();
      input_.direction = input->direction();
      last_sequence_ = input->sequence();
      return true;
    }
    else return false;
  }
  else
    return false;
}

/**
 * @brief Get client's player input.
 * @return Player's input.
*/
PlayerInput ClientUdp::Input() { return input_; }

}  // namespace upzp::client_com