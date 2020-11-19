#pragma once

#include "vehicle_types.h"
#include <string>

namespace upzp {

/**
 * @brief Basic client representation.
*/
class Client {
 protected:
  const std::string name_;
  const uint32_t id_;
  const VehicleType vehicle_;

 public:
  Client(const std::string name, const uint32_t id, const VehicleType vehicle);
  std::string Name() const;
  uint32_t Id() const;
  VehicleType Vehicle() const;
};

}  // namespace upzp
