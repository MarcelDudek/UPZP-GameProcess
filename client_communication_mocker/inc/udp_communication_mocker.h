#pragma once

#include "datagram.h"
#include <string>

namespace upzp::mocker {

Datagram MockInputDatagram(std::size_t seq, uint32_t id, bool move, float dir);
std::string DecodeGameStatus(const char *buffer, const size_t size);

}  // namespace upzp::mocker
