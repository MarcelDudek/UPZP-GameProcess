#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_MAIN_PROCESS_COMM_MAIN_PROCESS_COMM_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_MAIN_PROCESS_COMM_MAIN_PROCESS_COMM_H_

#include <asio.hpp>
#include <string>
#include <thread>
#include <memory>
#include <datagram_stream.h>
#include <client_communication.h>
#include <game_logic.h>

namespace upzp::main_process_comm {

class MainProcessComm {
 private:
  static constexpr auto GAME_CLIENTS_VERSION = 10;

  asio::io_context context_;
  asio::ip::tcp::socket socket_;
  std::thread run_thread_;
  bool running_ = false;
  const std::string ip_v4_;
  const uint16_t port_;
  const uint32_t game_id_;
  std::vector<char> buffer_;
  DatagramStream datagram_stream_;

  std::shared_ptr<client_com::ClientCommunication> client_comm_;
  std::shared_ptr<game_logic::GameLogic> game_logic_;

  void Connect();
  void LoadClients();
  void StartReceive();
  void TransmitGameId();

 public:
  MainProcessComm(std::string address, uint16_t port, uint32_t game_id);
  virtual ~MainProcessComm();
  void Start();
  void Stop();
  void AssignClientCommunication(std::shared_ptr<client_com::ClientCommunication>);
  void AssignGameLogic(std::shared_ptr<game_logic::GameLogic>);
  void SendGameFinished();
};

}  // namespace upzp::main_process_comm

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_MAIN_PROCESS_COMM_MAIN_PROCESS_COMM_H_
