#pragma once

#include "client_udp.h"
#include <asio.hpp>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

namespace upzp::client_com {

/**
 * @brief Client communication main class.
*/
class ClientCommunication {
 private:
  static constexpr std::size_t RECEIVE_BUFFER_SIZE = 4096;
  static constexpr int GAME_STATUS_VER = 100;
  static constexpr int PLAYER_INPUT_VER = 101;
  const std::chrono::milliseconds game_status_period_;

  std::thread run_thread_;
  bool running_;

  uint64_t sequence_number_;

  asio::io_context context_;
  asio::ip::udp::socket socket_;
  asio::ip::udp::endpoint remote_endpoint_;
  asio::high_resolution_timer transmit_timer_;
  std::vector<ClientUdp> clients_;
  std::vector<char> receive_buffer_;
  std::vector<char> transmit_buffer_;

  // mutexes
  std::mutex clients_mutex_;

  void GenerateGameStatus(uint64_t seq_num);

  void StartReceive();
  void StartTransmit();

 public:
  ClientCommunication(const unsigned int port);
  void AddClient(Client client);
  void AddClient(std::vector<Client>& client);
  void Start();
};

}  // namespace upzp::client_com
