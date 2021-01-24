#include "inc/udp_communication_mocker.h"
#include "player_input_generated.h"
#include "game_status_generated.h"

namespace upzp::mocker {

Datagram MockInputDatagram(std::size_t seq, uint32_t id, bool move, float dir) {
  Datagram retval;
  retval.SetVersion(101);
  retval.SetPayloadChecksum(false);

  flatbuffers::FlatBufferBuilder builder(1024);
  auto input = Upzp::PlayerInput::CreateInput(builder, seq, id, false, dir, move);
  builder.Finish(input);
  retval.SetPayload(reinterpret_cast<char*>(builder.GetBufferPointer()), builder.GetSize());
  return retval;
}

std::string DecodeGameStatus(const char *buffer, const size_t size) {
  std::string ret_val = "not correct version";
  Datagram datagram;
  datagram.Load(buffer, size);
  if (datagram.Version() == 100) {
    auto data = datagram.Payload();
    auto game = Upzp::GameStatus::GetGame(data.data());
    auto teams_count = game->teams()->size();
    decltype(teams_count) players_count = 0;
    for (size_t i = 0; i < game->teams()->size(); i++) {
      players_count += game->teams()->Get(i)->players()->size();
    }
    auto seq = game->sequence();
    auto point_boxes_count = game->boxes()->size();
    auto finished = game->finished();

    ret_val = "teams count: " + std::to_string(teams_count) + "; players count: "
        + std::to_string(players_count) + "; point boxes count: "
        + std::to_string(point_boxes_count) + "; finished: " + std::to_string(finished)
        + "; seq: " + std::to_string(seq);
  }
  return ret_val;
}

}  // namespace upzp::mocker
