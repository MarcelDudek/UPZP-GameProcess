#include "tcp_communication_mocker.h"

namespace upzp::mocker {

/**
 * @brief Mock game.fbs with game player base.
 * @param team_1_player_count How many players in team red?
 * @param team_2_player_count How many players in team blue?
 * @return Datagram with message.
 */
Datagram MockPlayerListDatagram(std::size_t team_1_player_count, std::size_t team_2_player_count) {
  Datagram ret_val;
  ret_val.SetVersion(60);
  const char payload[] = "Testing payload.";
  ret_val.SetPayloadChecksum(true);
  ret_val.SetPayload(payload, sizeof(payload));
  return ret_val;
}

}