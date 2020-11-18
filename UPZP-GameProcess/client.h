#pragma once

#include "vehicle_types.h"
#include <string>

namespace upzp {

/**
 * @brief Basic client representation.
*/
struct Client {
  std::string ip_adress;
  std::string name;
  uint32_t id;
  VehicleType vehicle;
};

}  // namespace upzp
