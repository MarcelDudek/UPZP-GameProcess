#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_H_

#include <random>
#include "coordinates.h"
#include "Team.h"

namespace upzp::game_logic {

class Game {
 private:
  static constexpr double EARTH_RADIUS = 6371000.0;  // in meters

  const int64_t points_to_win_;
  const Coordinates map_placement_;
  const double map_radius_;
  std::random_device rd_;
  std::mt19937 generator_;
  std::uniform_real_distribution<double> distribution_angle_;
  std::normal_distribution<double> distribution_distance_;

 public:
  Team red_team_, blue_team_;

  Game(int64_t points_to_win, Coordinates map_placement, double map_radius);
  void SetPlayerInput(PlayerInput input);
  void AddPlayer(std::string name, uint32_t id, VehicleType vehicle, bool to_red_team);
  bool RedTeamWon() const;
  bool BlueTeamWon() const;
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_H_
