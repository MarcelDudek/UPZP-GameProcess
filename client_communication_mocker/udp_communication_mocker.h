#pragma once

#include "datagram.h"

namespace upzp::mocker {

Datagram MockInputDatagram(std::size_t seq, uint32_t id, bool move, float dir);

}  // namespace upzp::mocker
