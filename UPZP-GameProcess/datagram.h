#pragma once
#include <cstdint>
#include <vector>

namespace upzp {

/**
 * @brief Datagram for UDP and TCP communication.
*/
class Datagram {
 private:
  static constexpr auto BEGIN_SEQUENCE = 0xABDA;

  int16_t version_;
  uint32_t payload_lenght_;
  int32_t payload_checksum_;
  bool payload_checksum_included_;

  std::vector<char> payload_;

 public:
  void Load(const void* buffer, const size_t length);
  bool PayloadCorrectness() const;
  std::vector<char> Payload() const;

  void SetVersion(int16_t version);
  void SetPayloadChecksum(bool include_payload_checksum);
  void SetPayload(const char* buffer, std::size_t length);

  std::vector<char> Get() const;
};

}  // namespace upzp
