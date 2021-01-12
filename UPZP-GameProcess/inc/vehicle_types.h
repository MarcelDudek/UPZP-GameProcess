#pragma once

namespace upzp {

/**
 * @brief Possible vehicle types.
*/
enum class VehicleType {
  PEDESTRIAN,
  CYCLIST,
  CAR
};

/**
 * @brief Get vehicle speed.
 * @param type Vehicle type.
 * @return Speed in m/s.
 */
constexpr double VehicleSpeed(VehicleType type) {
  switch (type) {
    case VehicleType::PEDESTRIAN: return 5.0;
    case VehicleType::CYCLIST: return 9.0;
    case VehicleType::CAR: return 13.88;
    default: return 0.0;
  }
}

}  // namespace upzp
