#include "inc/Team.h"

namespace upzp::game_logic {

/**
 * @brief Add player to the team.
 * @param player Player to be added.
 */
void Team::AddPlayer(const Player& player) {
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
void Team::SetPlayerInput(PlayerInput& input) {
  for (auto& player : players_) {
    if (player.Id() == input.id) {
      player.SetInput(input);
      return;
    }
  }
}

/**
 * Generate team data serialization using flatbuffers.
 *
 * @brief Generate flatbuffers reprezentation of a team.
 * @param builder Flatbuffers builder.
 * @return Flatbuffers offset.
 */
flatbuffers::Offset<Upzp::GameStatus::Team>
    Team::GenerateFlatbuffers(flatbuffers::FlatBufferBuilder& builder) const {
  std::vector<flatbuffers::Offset<Upzp::GameStatus::Player>> players_vec;
  for (auto& player : players_) {
    players_vec.push_back(player.GenerateFlatbuffers(builder));
  }
  auto players = builder.CreateVector(players_vec);

  return Upzp::GameStatus::CreateTeam(builder, Score(), players);
}

}  // namespace upzp::game_logic