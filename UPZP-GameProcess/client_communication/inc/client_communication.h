#pragma once

#include "client_udp.h"
#include "game_logic.h"
#include <asio.hpp>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

namespace upzp::client_com {

/**
 * Client communication is receiving player's input
 * and transmitting game status to and from all clients.
 *
 * @brief Client communication main class.
 * @todo Add client disconnect and timeout handling.
*/
class ClientCommunication {
 private:
  static constexpr std::size_t RECEIVE_BUFFER_SIZE = 4096;
  static constexpr int GAME_STATUS_VER = 100;
  static constexpr int PLAYER_INPUT_VER = 101;
  const std::chrono::milliseconds game_status_period_;

  std::thread run_thread_;
  bool running_ = false;

  std::shared_ptr<game_logic::GameLogic> game_logic_;

  asio::io_context context_;
  asio::ip::udp::socket socket_;
  asio::ip::udp::endpoint remote_endpoint_;
  asio::high_resolution_timer transmit_timer_;
  std::vector<ClientUdp> clients_;
  std::vector<char> receive_buffer_;
  std::vector<char> transmit_buffer_;

  // mutexes
  std::mutex clients_mutex_;

  void GetGameStatusIntoTransmitBuffer();

  void StartReceive();
  void StartTransmit();

 public:
  explicit ClientCommunication(const unsigned int port);
  void AddClient(Client client);
  void AddClient(std::vector<Client>& client);
  void AssignGameLogic(std::shared_ptr<game_logic::GameLogic>);
  void Start();
};

}  // namespace upzp::client_com
