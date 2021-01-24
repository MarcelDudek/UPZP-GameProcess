#pragma once
#include <cstdint>

namespace upzp {

/**
 * @brief Player's input.
*/
struct PlayerInput {
  uint32_t id;        /**< player's ID */
  bool move = false;  /**< should the vehicle move */
  float direction;    /**< direction in radians (0 is north) */
};

}  // namespace upzp
