#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_H_

#include <random>
#include <vector>
#include <chrono>
#include "coordinates.h"
#include "Team.h"
#include "point_box.h"
#include "game_status_generated.h"
#include "mysql_connection.h"

namespace upzp::game_logic {

class Game {
 private:
  static constexpr double EARTH_RADIUS = 6371000.0;  // in meters
  static constexpr double EARTH_CIRCUMFERENCE = 40075000.0;  // in meters
  static constexpr int64_t POINT_BOX_VALUE = 100;
  static constexpr double POINT_BOX_COLLECT_DISTANCE = 5.0;  // in meters

  Team red_team_, blue_team_;  /**< Teams of the game. */

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
  [[nodiscard]] double PointBoxDistance(const Coordinates&, const Coordinates&, const Coordinates&) const;
  [[nodiscard]] static double LatitudeToMeters(double latitude) ;
  [[nodiscard]] static double LongitudeToMeters(double longitude, double latitude) ;

 public:
  Game(int64_t points_to_win, Coordinates map_placement, double map_radius);
  void SetPlayerInput(PlayerInput& input);
  void AddPlayer(std::string name, uint32_t id, VehicleType vehicle, bool to_red_team);
  void GeneratePointBox();
  void GenerateFlatbuffers(flatbuffers::FlatBufferBuilder&, uint64_t) const;
  void CalculateMovement(std::chrono::duration<double>);
  [[nodiscard]] bool RedTeamWon() const;
  [[nodiscard]] bool BlueTeamWon() const;
  [[nodiscard]] int64_t RedTeamScore() const;
  [[nodiscard]] int64_t BlueTeamScore() const;
  void CreatePlayersTableStatement(sql::PreparedStatement **prepared_stmt, sql::Connection* conn,
                                   uint32_t game_id, const std::string& map_name) const;
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_H_
