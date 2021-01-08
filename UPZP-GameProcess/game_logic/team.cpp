#include "inc/Team.h"

namespace upzp::game_logic {

/**
 * @brief Add player to the team.
 * @param player Player to be added.
 */
void Team::AddPlayer(Player player) {
  players_.push_back(player);
}

/**
 * @brief Get team's score,
 * @return Points count.
 */
int64_t Team::Score() const {
  int64_t score = 0;
  for (auto& player : players_) {
    score += player.Points();
  }
  return score;
}

/**
 * @brief Set player's input.
 * @param input Input.
 */
void Team::SetPlayerInput(PlayerInput input) {
  for (auto& player : players_) {
    if (player.Id() == input.id) {
      player.SetInput(input);
      return;
    }
  }
}

}  // namespace upzp::game_logic