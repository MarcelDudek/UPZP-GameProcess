#include "inc/client_communication.h"
#include "../datagram/inc/datagram.h"
#include <iostream>
#include <utility>

namespace upzp::client_com {

  /**
 * @brief Constructor.
 * @param port Port on which communication will be open.
 */
ClientCommunication::ClientCommunication(const unsigned int port)
    : receive_buffer_(RECEIVE_BUFFER_SIZE), transmit_timer_(context_),
    socket_(context_, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
    game_status_period_(50) {
}

/**
 * @brief Destructor.
 */
ClientCommunication::~ClientCommunication() {
  Stop();
}

/**
 * @brief Add client.
 * @param client Client to add.
*/
void ClientCommunication::AddClient(Client client) {
  clients_mutex_.lock();
  clients_.emplace_back(client);
  clients_mutex_.unlock();
}

/**
 * @brief Add multiple clients.
 * @param client List of clients.
*/
void ClientCommunication::AddClient(std::vector<Client>& clients) {
  clients_mutex_.lock();
  for (auto& client : clients) {
    clients_.emplace_back(client);
  }
  clients_mutex_.unlock();
}

/**
 * @brief Start the client communication.
 */
void ClientCommunication::Start() {
  if (!running_) {
    running_ = true;
    StartReceive();
    StartTransmit();
    run_thread_ = std::thread([this]() {
      context_.run();
      std::cout << "Client communication terminated.\n";
      running_ = false;
      });
  }
}

/**
 * @brief Start receiving thread.
*/
void ClientCommunication::StartReceive() {
  socket_.async_receive_from(
      asio::buffer(receive_buffer_.data(),receive_buffer_.size()),
      remote_endpoint_,
    [this](const asio::error_code& error, std::size_t bytes_transferred) {  // handler
      clients_mutex_.lock();
      for (auto& client : clients_) {  // check all clients for corresponding address
        if (remote_endpoint_.address().to_string() == client.Ip_v4()
        && remote_endpoint_.port() == client.Port()) {
          try {
            bool good_read = client.DecodeDatagram(receive_buffer_.data(), bytes_transferred);
            if (good_read && game_logic_)
              game_logic_->SetPlayerMovement(client.Input());
          }
          catch (std::exception& ex) {
            std::cerr << "Exception during decoding of client datagram: " << ex.what() << std::endl;
          }
          break;
        }
      }
      clients_mutex_.unlock();

      StartReceive();  // start new receive
    });
}

/**
 * @brief Start transmitting thread.
*/
void ClientCommunication::StartTransmit() {
  transmit_timer_.expires_after(game_status_period_);
  transmit_timer_.async_wait([this](const asio::error_code& error) {
    if (!game_logic_)  // if there is no loaded game logic, abort sending game status
      StartTransmit();
    GetGameStatusIntoTransmitBuffer();
    clients_mutex_.lock();
    for (auto& client : clients_) {  // send game status to every client
      socket_.async_send_to(
          asio::buffer(transmit_buffer_.data(),transmit_buffer_.size()),
          client.remote_endpoint_,
        [](const asio::error_code& error, std::size_t bytes_transferred) {  // handler
          if (error)
            std::cerr << "Client communicator error during transmission: " << error.message()
            << std::endl;
        });
    }
    clients_mutex_.unlock();

    StartTransmit();  // start new transmit
    });
}

/**
 * Serialize game status using flatbuffers. Then pack
 * it into datagram and into the transmit buffer.
 *
 * @brief Get game status info into transmit buffer.
*/
void ClientCommunication::GetGameStatusIntoTransmitBuffer() {
  if (game_logic_) {
    flatbuffers::FlatBufferBuilder builder(4096);
    game_logic_->GetGameStatus(builder);
    transmit_buffer_ = std::vector<char>(builder.GetSize());
    Datagram datagram;
    datagram.SetVersion(GAME_STATUS_VER);
    datagram.SetPayload(reinterpret_cast<const char*>(builder.GetBufferPointer()),
                        builder.GetSize());
    datagram.SetPayloadChecksum(true);
    transmit_buffer_ = datagram.Get();
  }
}

/**
 * Assign game logic handler which will be used to get
 * game status from as well as to give players' input.
 *
 * @brief Assign game logic handler to the communication.
 * @param logic Game logic handler shared pointer.
 */
void ClientCommunication::AssignGameLogic(std::shared_ptr<game_logic::GameLogic> logic) {
  game_logic_ = std::move(logic);
}

/**
 * @brief Stop the client communication.
 */
void ClientCommunication::Stop() {
  if (running_) {
    socket_.cancel();
    socket_.close();
    context_.stop();
  }
  if (run_thread_.joinable())
    run_thread_.join();
}

}  // namespace upzp::client_com
