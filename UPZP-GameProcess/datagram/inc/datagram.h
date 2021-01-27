#pragma once
#include <cstdint>
#include <vector>

namespace upzp {

/**
 * @brief Datagram for UDP and TCP communication.
*/
class Datagram {
 private:
  int16_t version_;
  uint32_t payload_length_;
  int32_t payload_checksum_;
  bool payload_checksum_included_;

  std::vector<char> payload_;

 protected:
  static constexpr auto BEGIN_SEQUENCE = 0xABDA;

 public:
  void Load(const void* buffer, size_t length);
  [[nodiscard]] bool PayloadCorrectness() const;
  [[nodiscard]] std::vector<char> Payload() const;
  [[nodiscard]] int Version() const;

  void SetVersion(int16_t version);
  void SetPayloadChecksum(bool include_payload_checksum);
  void SetPayload(const char* buffer, std::size_t length);

  [[nodiscard]] std::vector<char> Get() const;
};

}  // namespace upzp
