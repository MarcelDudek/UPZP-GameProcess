#include "datagram.h"

#include <stdexcept>
#include <string>

#include "CRC.h"

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
  version_ = *(static_cast<const int16_t*>(buffor) + 1) & 0x00FF;
  payload_checksum_included_ =
      *(static_cast<const uint8_t*>(buffor) + 3) & 0b10000000;
  payload_lenght_ = *(static_cast<const uint32_t*>(buffor) + 1);

  unsigned short header_lenght;
  int16_t header_checksum;
  const char* payload_start;

  // load checksums and payload length
  if (payload_checksum_included_) {  // if payload checksum is inluded
    if (length < MINIMAL_LENGTH + 4) {
      throw std::runtime_error(
          "Datagram length " + std::to_string(length) +
          " is shorter than length of header with payload checksum included" +
          std::to_string(MINIMAL_LENGTH + 4) + ".");
    }

    payload_checksum_ = *(static_cast<const uint32_t*>(buffor) + 2);
    header_checksum = *(static_cast<const int16_t*>(buffor) + 7);
    payload_start = static_cast<const char*>(buffor) + 16;
    header_lenght = 16;
  } else {  // if payload checksum isn't included
    header_checksum = *(static_cast<const int16_t*>(buffor) + 5);
    payload_start = static_cast<const char*>(buffor) + 12;
    header_lenght = 12;
  }

  // check if header checksum is correct
  int16_t header_crc =
      CRC::Calculate(buffor, header_lenght - 2, CRC::CRC_16_ARC());
  if (header_checksum != header_crc) {
    throw std::runtime_error(
        "Header checksum is not equal to calculated CRC checksum.");
  }

  // load payload
  if (payload_lenght_ + header_lenght > length) {
    throw std::runtime_error("Payload length overflows the datagram.");
  }
  payload_ =
      std::vector<char>(payload_start, payload_start + payload_lenght_);
}

/**
 * Check payload correctness by checking if the checksum
 * and length are correct.
 *
 * @brief Check if payload is correct.
 * @return True for correct payload.
 */
bool Datagram::PayloadCorrectness() const {
  if (payload_lenght_ != payload_.size()) return false;

  if (payload_checksum_included_) {
    int32_t crc =
        CRC::Calculate(payload_.data(), payload_.size(), CRC::CRC_32());
    if (crc != payload_checksum_) return false;
  }

  return true;
}

/**
 * @brief Get payload data.
 * @return Payload data.
 */
std::vector<char> Datagram::Payload() const { return payload_; }

}  // namespace upzp
