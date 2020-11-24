#pragma once
#include <cstdint>
#include <vector>

namespace upzp {

class Datagram {
 private:
  static constexpr auto BEGIN_SEQUENCE = 0xABDA;

  int16_t version_;
  uint32_t payload_lenght_;
  int32_t payload_checksum_;
  bool payload_checksum_included_;

  std::vector<const char> payload_;

 public:
  void Load(const void* buffor, const size_t length);
  bool PayloadCorrectness() const;
  std::vector<const char> Payload() const;
};

}  // namespace upzp
