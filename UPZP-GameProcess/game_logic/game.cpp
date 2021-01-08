#include "inc/game.h"
#include <cmath>

namespace upzp::game_logic {

/**
 * @brief Constructor.
 * @param points_to_win How many points are needed for a team to win a game.
 * @param map_placement Map location.
 * @param map_radius Map radius from center point (map_placement) in meters.
 */
Game::Game(int64_t points_to_win, Coordinates map_placement, double map_radius) :
  points_to_win_(points_to_win), map_placement_(map_placement), map_radius_(map_radius),
  generator_(rd_()), distribution_angle_(0.0, 2 * M_PI),
  distribution_distance_(map_radius_ / 2.0, map_radius_ / 2.1) {
}

/**
 * @brief Set player input.
 * @param input Input.
 */
void Game::SetPlayerInput(PlayerInput input) {
  red_team_.SetPlayerInput(input);
  blue_team_.SetPlayerInput(input);
}

/**
 * @brief Add player to the game.
 * @param name Player's name.
 * @param id Player's ID.
 * @param vehicle Player's vehicle type.
 * @param to_red_team Should the player be added to the red team (otherwise will be added to blue)?
 */
void Game::AddPlayer(std::string name, uint32_t id, VehicleType vehicle, bool to_red_team) {
  auto distance = distribution_distance_(generator_);
  auto angle = distribution_angle_(generator_);

  auto dx = distance * std::cos(angle);
  auto dy = distance * std::sin(angle);
  Coordinates player_coords {
    map_placement_.longitude + (dx / EARTH_RADIUS) *
      (180.0 / M_PI) / std::cos(map_placement_.latitude * M_PI / 180.0),
    map_placement_.latitude + (dy / EARTH_RADIUS) * (180.0 / M_PI)
  };
  Player new_player(name, id, vehicle);
  new_player.SetPosition(player_coords);

  to_red_team ? red_team_.AddPlayer(new_player) : blue_team_.AddPlayer(new_player);
}

/**
 * @brief Did a red team win the game?
 * @return Boolean.
 */
bool Game::RedTeamWon() const {
  return red_team_.Score() >= points_to_win_;
}

/**
 * @brief Did a blue team win the game?
 * @return Boolean.
 */
bool Game::BlueTeamWon() const {
  return blue_team_.Score() >= points_to_win_;
}

}  // namespace upzp::game_logic
