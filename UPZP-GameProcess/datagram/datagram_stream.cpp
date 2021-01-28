#include <iostream>
#include "datagram_stream.h"
#include "CRC.h"

namespace upzp {

/**
 * Add data to datagram stream. It stream will complete
 * full datagram, it will be decoded.
 *
 * @brief Add stream data.
 * @param data Data buffer.
 * @param size Data buffer size.
 * @return True if complete datagram has been completed
 * and decoded.
 */
bool DatagramStream::AddData(const char* data, std::size_t size) {
  if (size > 0)
    buffer_.insert(buffer_.end(), data, data + size);
  return FlushData();
}

/**
 * Find begin sequence in the buffer.
 *
 * @brief Find begin sequence.
 * @return Position of begin sequence in the buffer.
 * Max value if position hasn't been found.
 */
std::size_t DatagramStream::FindBeginSequence() {
  static constexpr char LOWER = static_cast<char>(BEGIN_SEQUENCE & 0xFF);
  static constexpr char UPPER = static_cast<char>(BEGIN_SEQUENCE >> 8);

  for (std::size_t i = 0; i < buffer_.size() - 1; i++) {
    if (buffer_[i] == LOWER && buffer_[i + 1] == UPPER)
      return i;
  }

  return NOT_FOUND;
}

/**
 * Checks is header is correct. Method checks
 * header checksum and basic structure.
 *
 * @brief Is header correct?
 * @return Header is correct.
 */
bool DatagramStream::HeaderCorrect() {
  if (buffer_.size() < header_length_)
    return false;

  static constexpr char LOWER = static_cast<char>(BEGIN_SEQUENCE & 0xFF);
  static constexpr char UPPER = static_cast<char>(BEGIN_SEQUENCE >> 8);
  if (buffer_[0] != LOWER || buffer_[1] != UPPER)
    return false;

  int16_t header_checksum = *reinterpret_cast<int16_t*>(buffer_.data() + header_length_ - 2);
  int16_t header_crc =
      CRC::Calculate(buffer_.data(), header_length_ - 2, CRC::CRC_16_ARC());

  return header_checksum == header_crc;
}

/**
 * It will try to flush and complete the datagram.
 *
 * @brief Flush data and decode.
 * @return Datagram loaded.
 */
bool DatagramStream::FlushData() {
  bool repeat;
  do {
    repeat = false;

    // begin sequence ****
    if (state_ == State::BEGIN_SEQ) {
      auto pos = FindBeginSequence();
      if (pos == NOT_FOUND) {
        // erase all but last (might be first byte of begin seq)
        buffer_.erase(buffer_.begin(), buffer_.end() - 1);
      } else {
        // erase to the begin seq point
        if (pos > 0)
          buffer_.erase(buffer_.begin(), buffer_.begin() + pos);
        state_ = State::HEADER_1;
      }
    }

    // header 1 ****
    if (state_ == State::HEADER_1 && buffer_.size() >= 12) {
      header_length_ = (buffer_[3] >> 7) ? 16 : 12;
      state_ = State::HEADER_2;
    }

    // header 2 ****
    if (state_ == State::HEADER_2 && buffer_.size() >= header_length_) {
      if (HeaderCorrect()) {
        payload_length_ = *reinterpret_cast<uint32_t*>(buffer_.data() + 4);
        state_ = State::PAYLOAD;
      } else {
        std::cout << "Incorrect header!\n";
        buffer_.erase(buffer_.begin());
        state_ = State::BEGIN_SEQ;
        repeat = true;
      }
    }

    // payload ****
    if (state_ == State::PAYLOAD && buffer_.size() >= header_length_ + payload_length_) {
      try {
        Load(buffer_.data(), header_length_ + payload_length_);
        buffer_.erase(buffer_.begin(), buffer_.begin() + header_length_ + payload_length_);
        state_ = State::BEGIN_SEQ;
        return true;
      } catch (std::exception& ex) {
        buffer_.erase(buffer_.begin());
        state_ = State::BEGIN_SEQ;
        repeat = true;
      }
    }
  } while (repeat);

  return false;
}

}  // namespace
