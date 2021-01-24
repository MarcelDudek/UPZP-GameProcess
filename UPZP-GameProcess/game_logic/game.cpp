#include "inc/game.h"
#include <cmath>
#include <utility>
#include <cppconn/prepared_statement.h>

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
void Game::SetPlayerInput(PlayerInput& input) {
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
  Player new_player(std::move(name), id, vehicle);
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

/**
 * @brief Generate point box in random location on the map.
 */
void Game::GeneratePointBox() {
  auto distance = distribution_distance_(generator_);
  auto angle = distribution_angle_(generator_);

  auto dx = distance * std::cos(angle);
  auto dy = distance * std::sin(angle);
  Coordinates coords {
      map_placement_.longitude + (dx / EARTH_RADIUS) *
          (180.0 / M_PI) / std::cos(map_placement_.latitude * M_PI / 180.0),
      map_placement_.latitude + (dy / EARTH_RADIUS) * (180.0 / M_PI)
  };

  point_boxes_.push_back({coords, POINT_BOX_VALUE});
}

/**
 * @brief Calculate movement of all players.
 */
void Game::CalculateMovement(std::chrono::duration<double> time_diff) {
  for (auto& player : red_team_.players_)
    CalculatePlayerMovement(player, time_diff);
  for (auto& player : blue_team_.players_)
    CalculatePlayerMovement(player, time_diff);
}

/**
 * @brief Calculate player's movement.
 * @param player
 */
void Game::CalculatePlayerMovement(Player& player, std::chrono::duration<double>& time_diff) {
  const auto input = player.Input();
  const auto old_position = player.Position();
  auto new_position = old_position;

  if(input.move) {  // calculate new position
    auto distance = VehicleSpeed(player.Vehicle()) * time_diff.count();
    auto dx = distance * std::cos(input.direction);
    auto dy = distance * std::sin(input.direction);

    new_position = {
      old_position.longitude + (dx / EARTH_RADIUS) *
          (180.0 / M_PI) / std::cos(old_position.latitude * M_PI / 180.0),
      old_position.latitude + (dy / EARTH_RADIUS) * (180.0 / M_PI)
    };
    player.AddDistanceTraveled(distance);
  }

  player.AddPoints(CheckForPointBox(old_position, new_position));
  player.SetPosition(new_position);
}

/**
 * Check if player has collected any point boxes during
 * his movement.
 *
 * @brief Check for point boxes.
 * @param old_position Player's old position.
 * @param new_position Player's new position.
 * @return Number of points that player collected.
 */
int64_t Game::CheckForPointBox(const Coordinates& old_position, const Coordinates& new_position) {
  int64_t score = 0;

  for (int64_t i = 0; static_cast<std::size_t>(i) < point_boxes_.size(); i++) {
    auto distance = PointBoxDistance(point_boxes_[i].position, old_position, new_position);
    if (distance < POINT_BOX_COLLECT_DISTANCE) {  // point box has been collected
      score += point_boxes_[i].value;
      point_boxes_.erase(point_boxes_.begin() + i);  // remove point box
      i--;
    }
  }

  return score;
}

/**
 * Calculate rough approximation of point box's distance to
 * the segment created from an old and new player's position.
 *
 * @brief Calculate point box's distance to the movement segment.
 * @param point_box_position Point box's position.
 * @param old_position Player's old position.
 * @param new_position PLayer's new position.
 * @return Distance in meters.
 */
double Game::PointBoxDistance(const Coordinates & point_box_position,
                              const Coordinates & old_position,
                              const Coordinates & new_position) const {
  auto x = LongitudeToMeters(point_box_position.longitude, point_box_position.latitude);
  auto y = LatitudeToMeters(point_box_position.latitude);
  auto x1 = LongitudeToMeters(old_position.longitude, old_position.latitude);
  auto y1 = LatitudeToMeters(old_position.latitude);
  auto x2 = LongitudeToMeters(new_position.longitude, new_position.latitude);
  auto y2 = LatitudeToMeters(new_position.latitude);

  auto A = x - x1;
  auto B = y - y1;
  auto C = x2 - x1;
  auto D = y2 - y1;

  auto dot = A * C + B * D;
  auto len_sq = C * C + D * D;
  auto param = -1.0;
  if (len_sq != 0.0)
    param = dot / len_sq;

  double xx, yy;

  if (param < 0.0) {
    xx = x1;
    yy = y1;
  }
  else if (param > 1.0) {
    xx = x2;
    yy = y2;
  }
  else {
    xx = x1 + param * C;
    yy = y1 + param * D;
  }

  auto dx = x - xx;
  auto dy = y - yy;
  return std::sqrt(dx * dx + dy * dy);
}

/**
 * Calculate rough approximation of latitude
 * to meters.
 *
 * @brief Convert latitude to meters.
 * @param latitude In degrees.
 * @return Meters.
 */
double Game::LatitudeToMeters(double latitude) {
  return latitude * 111320.0;
}

/**
 * Calculate rough approximation of longitude
 * to meters.
 *
 * @brief Convert latitude to meters.
 * @param longitude In degrees.
 * @param latitude In degrees.
 * @return Meters.
 */
double Game::LongitudeToMeters(double longitude, double latitude) {
  return (EARTH_CIRCUMFERENCE * std::cos(latitude) / 360.0) * longitude;
}

/**
 * Serialize game status data using flatbuffers.
 *
 * @brief Generate flatbuffers game status.
 * @param builder Flatbuffers builder. The serialized data will be outputted
 * into this builder.
 * @param sequence_number Sequence number that will be put into the serialization.
 */
void Game::GenerateFlatbuffers(flatbuffers::FlatBufferBuilder& builder,
                               uint64_t sequence_number) const {
  // create both teams
  auto red_team = red_team_.GenerateFlatbuffers(builder);
  auto blue_team = blue_team_.GenerateFlatbuffers(builder);
  std::vector<flatbuffers::Offset<Upzp::GameStatus::Team>> teams_vector;
  teams_vector.push_back(red_team);
  teams_vector.push_back(blue_team);
  auto teams = builder.CreateVector(teams_vector);

  // create point boxes
  std::vector<flatbuffers::Offset<Upzp::GameStatus::PointBox>> point_boxes_vector;
  for (auto& box : point_boxes_) {
    auto box_position = Upzp::GameStatus::Position(box.position.longitude, box.position.latitude);
    point_boxes_vector.push_back(
        Upzp::GameStatus::CreatePointBox(builder, &box_position, box.value));
  }
  auto point_boxes = builder.CreateVector(point_boxes_vector);

  auto game = Upzp::GameStatus::CreateGame(
      builder, sequence_number, teams, point_boxes, RedTeamWon() || BlueTeamWon());
  builder.Finish(game);
}

/**
 * Create player table prepared statement. It will be put into
 * MySQL API structure for later execution.
 *
 * @brief Create players table statement.
 * @param prepared_stmt Pointer to prepared statement structure pointer.
 * @param conn MySQL connection pointer.
 */
void Game::CreatePlayersTableStatement(sql::PreparedStatement **prepared_stmt,
                                       sql::Connection *conn, uint32_t game_id,
                                       const std::string& map_name) const {
    std::string query = "INSERT INTO stat_player_game"
                        "(player_id, game_id, player_points, "
                        "team, vehicle, distance, team_points, map, won) values ";

    bool first = true;
    for ([[maybe_unused]] auto& player : red_team_.players_) {
      query += first ?  "(?, ?, ?, ?, ?, ?, ?, ?, ?)" : ", (?, ?, ?, ?, ?, ?, ?, ?, ?)";
      first = false;
    }
    for ([[maybe_unused]] auto& player : blue_team_.players_) {
      query += first ?  "(?, ?, ?, ?, ?, ?, ?, ?, ?)" : ", (?, ?, ?, ?, ?, ?, ?, ?, ?)";
      first = false;
    }

    *prepared_stmt = conn->prepareStatement(query);

    // set values
    unsigned int i = 0;
    for (auto& player : red_team_.players_) {
      (*prepared_stmt)->setInt(++i, player.Id());
      (*prepared_stmt)->setInt(++i, game_id);
      (*prepared_stmt)->setInt(++i, player.Points());
      (*prepared_stmt)->setString(++i, "red");
      switch (player.Vehicle()) {
        case VehicleType::PEDESTRIAN:
          (*prepared_stmt)->setString(++i, "walk");
          break;
        case VehicleType::CYCLIST:
          (*prepared_stmt)->setString(++i, "bike");
          break;
        case VehicleType::CAR:
          (*prepared_stmt)->setString(++i, "car");
          break;
        default:
          (*prepared_stmt)->setString(++i, "walk");
          break;
      }
      (*prepared_stmt)->setInt(++i, player.DistanceTraveled());
      (*prepared_stmt)->setInt(++i, red_team_.Score());
      (*prepared_stmt)->setString(++i, map_name);
      (*prepared_stmt)->setInt(++i, RedTeamWon());
    }
    for (auto& player : blue_team_.players_) {
      (*prepared_stmt)->setInt(++i, player.Id());
      (*prepared_stmt)->setInt(++i, game_id);
      (*prepared_stmt)->setInt(++i, player.Points());
      (*prepared_stmt)->setString(++i, "blue");
      switch (player.Vehicle()) {
        case VehicleType::PEDESTRIAN:
          (*prepared_stmt)->setString(++i, "walk");
          break;
        case VehicleType::CYCLIST:
          (*prepared_stmt)->setString(++i, "bike");
          break;
        case VehicleType::CAR:
          (*prepared_stmt)->setString(++i, "car");
          break;
        default:
          (*prepared_stmt)->setString(++i, "walk");
          break;
      }
      (*prepared_stmt)->setInt(++i, player.DistanceTraveled());
      (*prepared_stmt)->setInt(++i, blue_team_.Score());
      (*prepared_stmt)->setString(++i, map_name);
      (*prepared_stmt)->setInt(++i, BlueTeamWon());
    }
}

/**
 * Getter for red team's score.
 *
 * @brief Red team's score.
 * @return Points count.
 */
int64_t Game::RedTeamScore() const {
  return red_team_.Score();
}

/**
 * Getter for blue team's score.
 *
 * @brief Blue team's score.
 * @return Points count.
 */
int64_t Game::BlueTeamScore() const {
  return blue_team_.Score();
}

}  // namespace upzp::game_logic
