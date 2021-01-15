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
    mutex_.lock();
    game_->AddPlayer(client.Name(), client.Id(), client.Vehicle(), to_red_team);
    mutex_.unlock();
  }
}

/**
 * @brief Start the game thread.
 */
void GameLogic::StartGame() {
  if (!game_started_ && game_) {
    game_started_ = true;
    game_thread_ = std::thread([this](){
      do {  // game loop
        Tick();
        std::this_thread::sleep_for(tick_duration_);
      } while (!GameFinished());

      // when the game has finished
      // TODO add logic
      game_started_ = false;
    });
  }
}

/**
 * @brief Game tick.
 */
void GameLogic::Tick() {
  mutex_.lock();
  game_->CalculateMovement(std::chrono::duration_cast<std::chrono::duration<double>>(tick_duration_));
  serialization_seq_num_++;
  mutex_.unlock();
}

/**
 * Check if the game has finised which means
 * that one of the teams has won the game.
 *
 * @brief Check if the game has finished.
 * @return True if game finished.
 */
bool GameLogic::GameFinished() {
  mutex_.lock();
  bool ret_val = game_->BlueTeamWon() || game_->RedTeamWon();
  mutex_.unlock();
  return ret_val;
}

/**
 * Get game status serialization using flatbuffers.
 *
 * @brief Get game status.
 * @param builder Flatbuffers builder.
 */
void GameLogic::GetGameStatus(flatbuffers::FlatBufferBuilder& builder) {
  if (game_) {
    mutex_.lock();
    game_->GenerateFlatbuffers(builder, serialization_seq_num_);
    mutex_.unlock();
  }
}

/**
 * Set player's movement using received input.
 *
 * @brief Set player's movement.
 * @param input Player's input.
 */
void GameLogic::SetPlayerMovement(PlayerInput input) {
  if (game_) {
    mutex_.lock();
    game_->SetPlayerInput(input);
    mutex_.unlock();
  }
}

/**
 * Is the game thread running?
 *
 * @brief Check if the game is running.
 * @return True for game thread running.
 */
bool GameLogic::Running() {
  bool ret_val;
  mutex_.lock();
  ret_val =  game_started_;
  mutex_.unlock();
  return ret_val;
}

}  // namespace upsp::game_logic
