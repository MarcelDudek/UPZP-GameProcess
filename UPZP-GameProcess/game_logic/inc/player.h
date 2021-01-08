#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_PLAYER_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_PLAYER_H_

#include <string>
#include "coordinates.h"
#include "player_input.h"
#include "vehicle_types.h"

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
  Coordinates position_ {0.0, 0.0};
  PlayerInput input_;

 public:
  Player(const std::string name, const uint32_t id, const enum VehicleType vehicle);
  std::string Name() const;
  uint32_t Id() const;
  int64_t Points() const;
  Coordinates Position() const;
  VehicleType Vehicle() const;
  PlayerInput Input() const;
  void SetPosition(Coordinates coords);
  void SetInput(PlayerInput input);
  void AddPoints(int64_t points);
  void ResetScore();
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_PLAYER_H_
