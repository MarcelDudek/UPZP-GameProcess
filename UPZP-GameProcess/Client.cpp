#include "client.h"

namespace upzp {

/**
 * @brief Constructor.
 * @param name Client's game name (nick).
 * @param id Client's ID.
 * @param vehicle Client's game vehicle type.
*/
Client::Client(const std::string name, const uint32_t id, 
  const VehicleType vehicle) : name_(name), id_(id), vehicle_(vehicle) {}


/**
 * @brief Get client's name (nick).
 * @return Client's name.
*/
std::string Client::Name() const { return name_; }


/**
 * @brief Get client's ID.
 * @return Client's ID.
*/
uint32_t Client::Id() const { return id_; }


/**
 * @brief Get client's game vehicle type.
 * @return Client's vehicle type.
*/
VehicleType Client::Vehicle() const { return vehicle_; }

}  // namespace upzp
