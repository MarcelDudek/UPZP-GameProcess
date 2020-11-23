#include "datagram.h"
#include "CRC.h"
#include <stdexcept>
#include <string>

namespace upzp {

/**
 * @brief Load the datagram from buffor.
 * @param buffor 
 * @param lenght Length of the buffor.
*/
void Datagram::Load(const void* buffor, const size_t length) {
  constexpr size_t MINIMAL_LENGTH = 12;

  if (length < MINIMAL_LENGTH) {
    throw std::runtime_error("Datagram length " + std::to_string(length) +
                             " is shorter than minimal length of " +
        std::to_string(MINIMAL_LENGTH) + ".");
  }

  // load begin sequence
  uint16_t begin_seq = *static_cast<const uint16_t*>(buffor);
  if (begin_seq != BEGIN_SEQUENCE) {
    throw std::runtime_error("Begin sequence of datagram isn't correct.");
  }

  // load version and check if payload checksum is included
  version_ = *(static_cast<const int16_t*>(buffor) + 2) & 0x00FF;
  payload_checksum_included_ =
      *(static_cast<const uint8_t*>(buffor) + 3) & 0b10000000;
  payload_lenght_ = *(static_cast<const uint32_t*>(buffor) + 4);

  unsigned short header_lenght;
  int16_t header_checksum;
  const char* payload_start;

  // load checksums and payload length
  if (payload_checksum_included_) {  // if payload checksum is inluded
    if (length < MINIMAL_LENGTH + 4) {
      throw std::runtime_error("Datagram length " + std::to_string(length) +
                               " is shorter than length of header with payload checksum included" +
                               std::to_string(MINIMAL_LENGTH + 4) + ".");
    }

    payload_checksum_ = *(static_cast<const uint32_t*>(buffor) + 8);
    header_checksum = *(static_cast<const int16_t*>(buffor) + 14);
    payload_start = static_cast<const char*>(buffor) + 16;
    header_lenght = 16;
  } else {  // if payload checksum isn't included
    header_checksum = *(static_cast<const int16_t*>(buffor) + 10);
    payload_start = static_cast<const char*>(buffor) + 12;
    header_lenght = 12;
  }

  // check if header checksum is correct
  int16_t header_crc = CRC::Calculate(buffor, header_lenght, CRC::CRC_16_ARC());
  if (header_checksum != header_crc) {
    throw std::runtime_error(
        "Header checksum is not equal to calculated CRC checksum.");
  }

  // load payload
  if (payload_lenght_ + header_lenght > length) {
    throw std::runtime_error("Payload length overflows the datagram.");
  }
  payload_ =
      std::vector<const char>(payload_start, payload_start + payload_lenght_);
}

}  // namespace upzp
