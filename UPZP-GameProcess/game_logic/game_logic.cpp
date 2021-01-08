#include "inc/game_logic.h"

namespace upzp::game_logic {

/**
 * @brief Start a new game.
 * @param map Map to be played.
 */
void GameLogic::NewGame(Maps map) {
  Coordinates start_point {MapsLongitude(map), MapsLatitude(map)};
  game_ = std::make_unique<Game>(1000, start_point, 2000.0);
}

/**
 * @brief Add new player to the game.
 * @param client New player.
 * @param to_red_team Should be added to red team (otherwise will be added to blue)?
 */
void GameLogic::AddPlayer(Client client, bool to_red_team) {
  if (game_) {
    game_->AddPlayer(client.Name(), client.Id(), client.Vehicle(), to_red_team);
  }
}

}  // namespace upsp::game_logic
