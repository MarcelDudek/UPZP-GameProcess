#pragma once
#include <cstdint>

namespace upzp {

/**
 * @brief Player's input.
*/
struct PlayerInput {
  uint32_t id = 0;        /**< player's ID */
  bool move = false;      /**< should the vehicle move */
  float direction = 0.0;  /**< direction in radians (0 is north) */
};

}  // namespace upzp
