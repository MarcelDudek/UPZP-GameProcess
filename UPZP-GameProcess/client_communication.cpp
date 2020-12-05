#include "client_communication.h"
#include <thread>
#include <chrono>
#include <iostream>

namespace upzp::client_com {

  /**
 * @brief Constructor.
 * @param port Port on which communication will be open.
 */
ClientCommunication::ClientCommunication(asio::io_context& context,
                                         const unsigned int port)
    : socket_(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
      receive_buffer_(RECEIVE_BUFFER_SIZE) {}

/**
 * @brief Add client.
 * @param client Client to add.
*/
void ClientCommunication::AddClient(Client client) {
  clients_.push_back(client);
}

/**
 * @brief Add multiple clients.
 * @param client List of clients.
*/
void ClientCommunication::AddClient(std::vector<Client>& clients) {
  for (auto& client : clients) {
    clients_.push_back(client);
  }
}

/**
 * @brief Start the client communication.
 */
void ClientCommunication::Start() {
  StartReceive();
  StartTransmit();
}

/**
 * @brief Start receiving thread.
*/
void ClientCommunication::StartReceive() {
  socket_.async_receive_from(asio::buffer(receive_buffer_.data(), receive_buffer_.size()),
    remote_endpoint_,
    [this](const asio::error_code& error, std::size_t bytes_transfered) {
      //std::cout << "Received from " << remote_endpoint_.address().to_string()
      //  << " " << bytes_transfered << " bytes of data:\n";
      //for (size_t i = 0; i < bytes_transfered; i++)
      //  std::cout << buffer_[i];
      //std::cout << std::endl;

      for (auto& client : clients_) {
        if (remote_endpoint_.address().to_string() == client.Ip_v4() && remote_endpoint_.port() == client.Port()) {
          try {
            client.DecodeDatagram(receive_buffer_.data(), bytes_transfered);
          }
          catch (std::exception& ex) {}
          std::cout << "Received from " << remote_endpoint_.address().to_string() << ":" << remote_endpoint_.port()
            << " " << bytes_transfered << " bytes of data:\n";
          break;
        }
      }

      StartReceive();
    });
}

/**
 * @brief Start transmiting thread.
*/
void ClientCommunication::StartTransmit() {
  transmit_buffer_.push_back('t');
  transmit_buffer_.push_back('e');
  transmit_buffer_.push_back('s');
  transmit_buffer_.push_back('t');

  transmit_thread_ = std::thread([this]() {
    while (true) {
      for (auto& client : clients_) {
        socket_.async_send_to(asio::buffer(transmit_buffer_.data(), transmit_buffer_.size()), client.remote_endpoint_, [this](const asio::error_code& error, std::size_t bytes_transfered) {});
      }
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1000ms);
    }
    });
}

}  // namespace upzp::client_com
