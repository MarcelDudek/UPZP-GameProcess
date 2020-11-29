#include "client_udp.h"
#include "datagram.h"

namespace upzp::client_com {

/**
 * @brief Constructor.
 * @param name Client's name.
 * @param id Client's ID.
 * @param vehicle Client's vehicle type.
*/
ClientUdp::ClientUdp(const std::string name, const uint32_t id,
                     const VehicleType vehicle) : Client(name, id, vehicle) {
  player_input_ = nullptr;
}

/**
 * @brief Destructor.
*/
ClientUdp::~ClientUdp() { delete player_input_; }

/**
 * @brief Decode received datagram.
 * @param buffer Datagram's buffer.
 * @param size Buffer's size.
 * @return True for succesful decode.
*/
bool ClientUdp::DecodeDatagram(const char* buffer, const size_t size) {
  Datagram datagram;
  datagram.Load(buffer, size);

  if (datagram.Version() == DATAGRAM_PLAYER_INPUT_VER) {
    auto input = Upzp::PlayerInput::GetInput(buffer);
    if (input->sequence() > player_input_->sequence()) {  // if sequence is bigger (newer input)
      delete player_input_;
      player_input_ = input;
    } else {  // if the input is older, discard it
      delete input;
    }
  }
  else
    return false;

  return true;
}

/**
 * @brief Get client's player input.
 * @return Player's input.
*/
PlayerInput ClientUdp::Input() {
  PlayerInput retval;
  retval.id = this->id_;
  if (player_input_) {
    retval.move = player_input_->move();
    retval.direction = player_input_->direction();
  } else {
    retval.move = false;
    retval.direction = 0.0;
  }
  return retval;
}

}  // namespace upzp::client_com