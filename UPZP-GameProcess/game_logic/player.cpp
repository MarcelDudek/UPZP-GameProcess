#include <utility>

#include "inc/player.h"

namespace upzp::game_logic {

/**
 * @brief Constructor.
 * @param name Name of the player.
 * @param id ID of the player.
 */
Player::Player(std::string name, const uint32_t id, const enum VehicleType vehicle) :
  name_(std::move(name)), id_(id), vehicle_(vehicle) {
}

/**
 * @brief Set player's position on the map.
 * @param coords Position's coordinates.
 */
void Player::SetPosition(Coordinates coords) {
  position_ = coords;
}

/**
 * @brief Get player's name.
 * @return Player's name.
 */
std::string Player::Name() const {
  return name_;
}

/**
 * @brief Get player's ID.
 * @return Player's ID.
 */
uint32_t Player::Id() const {
  return id_;
}

/**
 * @brief Get player's current points.
 * @return Player's current points count.
 */
int64_t Player::Points() const {
  return points_;
}

/**
 * @brief Get player's current position.
 * @return Coordinates of current position.
 */
Coordinates Player::Position() const {
  return position_;
}

/**
 * @brief Player's vehicle.
 * @return Vehicle type.
 */
VehicleType Player::Vehicle() const {
  return vehicle_;
}

/**
 * @brief Get player's movement input.
 * @return Player's input.
 */
PlayerInput Player::Input() const {
  return input_;
}

/**
 * @brief Add points to player's score.
 * @param points Amount of points to add.
 */
void Player::AddPoints(int64_t points) {
  points_ += points;
}

/**
 * @brief Reset player's points score to 0.
 */
void Player::ResetScore() {
  points_ = 0;
}

/**
 * @brief Set player's input.
 * @param input New input.
 */
void Player::SetInput(PlayerInput& input) {
  input_ = input;
}

/**
 * Generate player serialization using flatbuffers.
 *
 * @brief Generate flatbuffers player representation.
 * @param builder Flatbuffers builder.
 * @return Player flatbuffers offset.
 */
flatbuffers::Offset<Upzp::GameStatus::Player>
    Player::GenerateFlatbuffers(flatbuffers::FlatBufferBuilder& builder) const {
  auto name = builder.CreateString(name_);
  auto position = Upzp::GameStatus::Position(position_.longitude, position_.latitude);

  // check vehicle type
  Upzp::GameStatus::Vehicle vehicle;
  switch (vehicle_) {
    case VehicleType::CYCLIST:
      vehicle = Upzp::GameStatus::Vehicle::Vehicle_Cyclist;
      break;
    case VehicleType::PEDESTRIAN:
      vehicle = Upzp::GameStatus::Vehicle::Vehicle_Pedestrian;
      break;
    case VehicleType::CAR:
      vehicle = Upzp::GameStatus::Vehicle::Vehicle_Car;
      break;
    default:
      vehicle = Upzp::GameStatus::Vehicle::Vehicle_Pedestrian;
      break;
  }

  return Upzp::GameStatus::CreatePlayer(builder, name, id_, points_, &position, vehicle);
}

/**
 * Add distance to already traveled distance
 * by the player.
 *
 * @brief Add distance traveled.
 * @param distance Distance to be added (in meters).
 */
void Player::AddDistanceTraveled(double distance) {
  if (distance > 0.0)
    distance_traveled_ += distance;
}

/**
 * Get the distance that player traveled during
 * the game.
 *
 * @brief Get distance traveled.
 * @return Distance in meters.
 */
double Player::DistanceTraveled() const {
  return distance_traveled_;
}

}  // namespace upzp::game_logic
