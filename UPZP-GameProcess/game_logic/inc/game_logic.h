#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_

#include <memory>
#include "game.h"
#include "maps.h"
#include "client.h"

namespace upzp::game_logic {

class GameLogic {
 private:
  std::unique_ptr<Game> game_;

 public:
  void NewGame(Maps map);
  void AddPlayer(Client client, bool to_red_team);
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_
