#include "inc/udp_communication_mocker.h"
#include "player_input_generated.h"

namespace upzp::mocker {

Datagram MockInputDatagram(std::size_t seq, uint32_t id, bool move, float dir) {
  Datagram retval;
  retval.SetVersion(101);
  retval.SetPayloadChecksum(false);

  flatbuffers::FlatBufferBuilder builder(1024);
  auto input = Upzp::PlayerInput::CreateInput(builder, seq, false, dir, move);
  builder.Finish(input);
  retval.SetPayload(reinterpret_cast<char*>(builder.GetBufferPointer()), builder.GetSize());
  return retval;
}

}  // namespace upzp::mocker
