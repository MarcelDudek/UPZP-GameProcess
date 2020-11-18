#include "client_communication.h"

namespace upzp::client_com {

/**
 * @brief Constructor.
 * @param port Port on which communication will be open.
*/
ClientCommunication::ClientCommunication(const unsigned int port)
  : port_(port) {
}

}