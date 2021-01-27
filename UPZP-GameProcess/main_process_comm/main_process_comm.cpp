#include "inc/main_process_comm.h"
#include <iostream>

namespace upzp::main_process_comm {

/**
 * @brief Constructor.
 * @param port Port for TCP communication.
 */
MainProcessComm::MainProcessComm(std::string address, uint16_t port) : context_(),
socket_(context_), ip_v4_(address), port_(port), buffer_(1024) {
}

/**
 * @brief Start main process communication in thread.
 */
void MainProcessComm::Start() {
  if (!running_) {
    running_ = true;
    run_thread_ = std::thread([this]() {
      try {
        Read();
      } catch (std::exception& ex) {
        std::cout << "Main process connection error: " << ex.what() << std::endl;
      }
      std::cout << "Main process connection terminated.\n";
      running_ = false;
    });
  }
}

/**
 * @brief Read data from TCP connection.
 */
void MainProcessComm::Read() {
  asio::error_code error_;
  asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip_v4_, error_), port_);
  if (error_)
    throw asio::system_error(error_);
  socket_.connect(endpoint, error_);
  if (error_)
    throw asio::system_error(error_);
  std::cout << "Connected to main process\n";

  while (error_ != asio::error::eof) {
    auto length = socket_.read_some(asio::buffer(buffer_), error_);
    if (error_ != asio::error::eof && error_)  // if there is a connection error
      throw asio::system_error(error_);
    if (datagram_stream_.AddData(buffer_.data(), length)) {
      std::cout << "Received datagram version " << datagram_stream_.Version() << "!!!\n";
      while (datagram_stream_.FlushData())
        std::cout << "Received datagram version " << datagram_stream_.Version() << "!!!\n";
    }
  }
}

}  // upzp::main_process_comm
