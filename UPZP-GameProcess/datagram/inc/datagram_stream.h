#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_DATAGRAM_INC_DATAGRAM_STREAM_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_DATAGRAM_INC_DATAGRAM_STREAM_H_

#include <datagram.h>
#include <vector>
#include <limits>

namespace upzp {

class DatagramStream : private Datagram {
 private:
  /**
   * @brief States of receiving datagram stream.
   */
  enum class State {
    BEGIN_SEQ, HEADER_1, HEADER_2, PAYLOAD
  };

  static constexpr auto NOT_FOUND = std::numeric_limits<std::size_t>::max();

  State state_ = State::BEGIN_SEQ;
  std::vector<char> buffer_;
  std::size_t header_length_ = 12;
  std::size_t payload_length_ = 0;

  std::size_t FindBeginSequence();
  bool HeaderCorrect();

 public:
  DatagramStream() = default;
  bool AddData(const char*, std::size_t);
  bool FlushData();
  using Datagram::Version;
  using Datagram::Payload;
  using Datagram::PayloadCorrectness;
};

}  // namespace upzp

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_DATAGRAM_INC_DATAGRAM_STREAM_H_
