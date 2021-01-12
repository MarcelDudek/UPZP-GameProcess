#include "inc/game_logic.h"

namespace upzp::game_logic {

/**
 * @brief Constructor.
 */
GameLogic::GameLogic() : tick_duration_(1000 / TICK_RATE) {
}

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

/**
 * @brief Start the game thread.
 */
void GameLogic::StartGame() {
  if (!game_started_) {
    game_started_ = true;
    game_thread_ = std::thread([this](){
      do {  // game loop
        Tick();
        std::this_thread::sleep_for(tick_duration_);
      } while (game_->BlueTeamWon() || game_->RedTeamWon());

      // when the game has finished
      // TODO add logic
    });
  }
}

/**
 * @brief Game tick.
 */
void GameLogic::Tick() {
  mutex_.lock();
  game_->CalculateMovement(std::chrono::duration_cast<std::chrono::duration<double>>(tick_duration_));
  mutex_.unlock();
}

}  // namespace upsp::game_logic
