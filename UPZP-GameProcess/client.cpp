#include "inc/client.h"

namespace upzp {

/**
 * @brief Constructor.
 * @param name Client's game name (nick).
 * @param id Client's ID.
 * @param vehicle Client's game vehicle type.
 * @param ip Client's IP v4.
*/
Client::Client(const std::string name, const uint32_t id, 
  const VehicleType vehicle, const std::string ip, const unsigned int port) : name_(name), id_(id), vehicle_(vehicle), ip_v4_(ip), port_(port) {}


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

/**
 * @brief Get client's IP.
 * @return IP v4.
*/
std::string Client::Ip_v4() const { return ip_v4_; }

/**
 * @brief Get client's communication port.
 * @return Port number.
*/
unsigned int Client::Port() const {
  return port_;
}

}  // namespace upzp
