#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_MAIN_PROCESS_COMM_MAIN_PROCESS_COMM_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_MAIN_PROCESS_COMM_MAIN_PROCESS_COMM_H_

#include <asio.hpp>
#include <string>
#include <thread>

namespace upzp::main_process_comm {

class MainProcessComm {
 private:
  asio::io_context context_;
  asio::ip::tcp::socket socket_;
  std::thread run_thread_;
  bool running_ = false;
  std::string ip_v4_;
  uint16_t port_;
  std::vector<char> buffer_;

  void Read();

 public:
  MainProcessComm(std::string address, uint16_t port);
  void Start();
};

}  // namespace upzp::main_process_comm

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_MAIN_PROCESS_COMM_MAIN_PROCESS_COMM_H_
