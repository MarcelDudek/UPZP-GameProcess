#include "inc/datagram.h"

#include <stdexcept>
#include <string>
#include <cstring>

#include "../inc/CRC.h"

namespace upzp {

/**
 * @brief Load the datagram from buffer.
 * @param buffer
 * 
 * @param lenght Length of the buffer.
 */
void Datagram::Load(const void* buffer, const size_t length) {
  constexpr size_t MINIMAL_LENGTH = 12;

  if (length < MINIMAL_LENGTH) {
    throw std::runtime_error("Datagram length " + std::to_string(length) +
                             " is shorter than minimal length of " +
                             std::to_string(MINIMAL_LENGTH) + ".");
  }

  // load begin sequence
  uint16_t begin_seq = *static_cast<const uint16_t*>(buffer);
  if (begin_seq != BEGIN_SEQUENCE) {
    throw std::runtime_error("Begin sequence of datagram isn't correct.");
  }

  // load version and check if payload checksum is included
  version_ = *(static_cast<const int16_t*>(buffer) + 1) & 0x00FF;
  payload_checksum_included_ =
      *(static_cast<const uint8_t*>(buffer) + 3) & 0b10000000;
  payload_length_ = *(static_cast<const uint32_t*>(buffer) + 1);

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

    payload_checksum_ = *(static_cast<const uint32_t*>(buffer) + 2);
    header_checksum = *(static_cast<const int16_t*>(buffer) + 7);
    payload_start = static_cast<const char*>(buffer) + 16;
    header_lenght = 16;
  } else {  // if payload checksum isn't included
    header_checksum = *(static_cast<const int16_t*>(buffer) + 5);
    payload_start = static_cast<const char*>(buffer) + 12;
    header_lenght = 12;
  }

  // check if header checksum is correct
  int16_t header_crc =
      CRC::Calculate(buffer, header_lenght - 2, CRC::CRC_16_ARC());
  if (header_checksum != header_crc) {
    throw std::runtime_error(
        "Header checksum is not equal to calculated CRC checksum.");
  }

  // load payload
  if (payload_length_ + header_lenght > length) {
    throw std::runtime_error("Payload length overflows the datagram.");
  }
  payload_ = std::vector<char>(payload_start, payload_start + payload_length_);
}

/**
 * Check payload correctness by checking if the checksum
 * and length are correct.
 *
 * @brief Check if payload is correct.
 * @return True for correct payload.
 */
bool Datagram::PayloadCorrectness() const {
  if (payload_length_ != payload_.size()) return false;

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

/**
 * @brief Get version.
 * @return Version number.
*/
int Datagram::Version() const { return version_; }

/**
 * @brief Set the version of the datagram.
 * @param version New version.
 */
void Datagram::SetVersion(int16_t version) { 
  version_ = version & 0x7FFF; 
}

/**
 * @brief Include payload checksum in datagram header.
 * @param include_payload_checksum Should be included?
*/
void Datagram::SetPayloadChecksum(bool include_payload_checksum) {
  payload_checksum_included_ = include_payload_checksum;
}

/**
 * @brief Set payload of the datagram.
 * @param buffer Payload's buffer.
 * @param length Buffer length.
*/
void Datagram::SetPayload(const char* buffer, std::size_t length) {
  if (length <= std::numeric_limits<uint32_t>::max()) {
    payload_ = std::vector<char>(buffer, buffer + length);
    payload_length_ = length;
  }
}

/**
 * @brief Get datagram.
 * @return Vector containing datagram data.
*/
std::vector<char> Datagram::Get() const {
  std::size_t length = 12 + payload_.size();
  if (payload_checksum_included_) length += 4;

  // begin seq, version and checksum include
  std::vector<char> retval(length);
  retval[0] = BEGIN_SEQUENCE & 0x00FFu;
  retval[1] = (BEGIN_SEQUENCE & 0xFF00u) >> 8;
  retval[2] = version_ & 0x00FF;
  retval[3] = (version_ & 0x7F00) >> 8;
  if (payload_checksum_included_) retval[3] |= 0x80;

  // payload length
  auto payload_length = reinterpret_cast<uint32_t*>(&retval[4]);
  *payload_length = payload_.size();

  // header checksum and payload start location
  int16_t* header_checksum;
  void* payload_start;
  if (payload_checksum_included_) {
    auto payload_checksum = reinterpret_cast<int32_t*>(&retval[8]);
    *payload_checksum =
      CRC::Calculate(payload_.data(), payload_.size(), CRC::CRC_32());  // calculate the payload checksum
    header_checksum = reinterpret_cast<int16_t*>(&retval[14]);
    payload_start = static_cast<void*>(&retval[16]);
    *header_checksum =
      CRC::Calculate(retval.data(), 14, CRC::CRC_16_ARC());  // calculate header checksum
  } else {
    header_checksum = reinterpret_cast<int16_t*>(&retval[10]);
    payload_start = static_cast<void*>(&retval[12]);
    *header_checksum =
      CRC::Calculate(retval.data(), 10, CRC::CRC_16_ARC());  // caluclate header checksum
  }
  
  // copy the payload
  std::memcpy(payload_start, payload_.data(), payload_.size());

  return retval; 
}

}  // namespace upzp
