#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_H_

#include <random>
#include <vector>
#include <chrono>
#include "coordinates.h"
#include "Team.h"
#include "point_box.h"

namespace upzp::game_logic {

class Game {
 private:
  static constexpr double EARTH_RADIUS = 6371000.0;  // in meters
  static constexpr double EARTH_CIRCUMFERENCE = 40075000.0;  // in meters
  static constexpr int64_t POINT_BOX_VALUE = 100;
  static constexpr double POINT_BOX_COLLECT_DISTANCE = 5.0;  // in meters

  const int64_t points_to_win_;
  const Coordinates map_placement_;
  const double map_radius_;
  std::random_device rd_;
  std::mt19937 generator_;
  std::uniform_real_distribution<double> distribution_angle_;
  std::normal_distribution<double> distribution_distance_;
  std::vector<PointBox> point_boxes_;

  void CalculatePlayerMovement(Player&, std::chrono::duration<double>&);
  int64_t CheckForPointBox(const Coordinates&, const Coordinates&);
  double PointBoxDistance(const Coordinates&, const Coordinates&, const Coordinates&) const;
  double LatitudeToMeters(double latitude) const;
  double LongitudeToMeters(double longitude, double latitude) const;

 public:
  Team red_team_, blue_team_;

  Game(int64_t points_to_win, Coordinates map_placement, double map_radius);
  void SetPlayerInput(PlayerInput input);
  void AddPlayer(std::string name, uint32_t id, VehicleType vehicle, bool to_red_team);
  void GeneratePointBox();
  void CalculateMovement(std::chrono::duration<double>);
  bool RedTeamWon() const;
  bool BlueTeamWon() const;
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_H_
