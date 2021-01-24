#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_TEAM_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_TEAM_H_

#include <vector>
#include "player.h"

namespace upzp::game_logic {

class Team {
 public:
  std::vector<Player> players_;

  void AddPlayer(const Player&);
  void SetPlayerInput(PlayerInput& input);
  int64_t Score() const;
  flatbuffers::Offset<Upzp::GameStatus::Team>
    GenerateFlatbuffers(flatbuffers::FlatBufferBuilder&) const;
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_TEAM_H_
