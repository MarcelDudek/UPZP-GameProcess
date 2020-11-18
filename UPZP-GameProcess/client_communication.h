#pragma once

#include "client.h"
#include <vector>

namespace upzp::client_com {

/**
 * @brief Client communication main class.
*/
class ClientCommunication {
 private:
  std::vector<Client> clients_;
  const unsigned int port_;

 public:
  explicit ClientCommunication(const unsigned int port);
};

}  // namespace upzp::client_com
