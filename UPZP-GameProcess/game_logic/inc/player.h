#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_PLAYER_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_PLAYER_H_

#include <string>
#include "coordinates.h"
#include "player_input.h"
#include "vehicle_types.h"
#include "game_status_generated.h"

namespace upzp::game_logic {

/**
 * Player of the team.
 */
class Player {
 protected:
  const std::string name_;
  const uint32_t id_;
  const VehicleType vehicle_;
  int64_t points_ = 0;
  double distance_traveled_ = 0.0;  // in meters
  Coordinates position_ {0.0, 0.0};
  PlayerInput input_;

 public:
  Player(std::string  name, uint32_t id, enum VehicleType vehicle);
  [[nodiscard]] std::string Name() const;
  [[nodiscard]] uint32_t Id() const;
  [[nodiscard]] int64_t Points() const;
  [[nodiscard]] Coordinates Position() const;
  [[nodiscard]] VehicleType Vehicle() const;
  [[nodiscard]] PlayerInput Input() const;
  [[nodiscard]] double DistanceTraveled() const;
  void SetPosition(Coordinates coords);
  void SetInput(PlayerInput& input);
  void AddPoints(int64_t points);
  void AddDistanceTraveled(double distance);
  void ResetScore();
  flatbuffers::Offset<Upzp::GameStatus::Player>
    GenerateFlatbuffers(flatbuffers::FlatBufferBuilder&) const;
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_PLAYER_H_
