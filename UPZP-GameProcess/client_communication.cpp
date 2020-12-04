#include "client_communication.h"
#include <iostream>

namespace upzp::client_com {

/**
 * @brief Constructor.
 * @param port Port on which communication will be open.
 */
ClientCommunication::ClientCommunication(asio::io_context& context,
                                         const unsigned int port)
    : socket_(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
      buffer_(RECEIVE_BUFFER_SIZE) {}

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
  socket_.async_receive_from(asio::buffer(buffer_.data(), buffer_.size()),
                             remote_endpoint_,
    [this](const asio::error_code& error, std::size_t bytes_transfered) {
        //std::cout << "Received from " << remote_endpoint_.address().to_string()
        //  << " " << bytes_transfered << " bytes of data:\n";
        //for (size_t i = 0; i < bytes_transfered; i++)
        //  std::cout << buffer_[i];
        //std::cout << std::endl;

        for (auto& client : clients_) {
          if (remote_endpoint_.address().to_string() == client.Ip_v4()) {
            client.remote_endpoint_ = remote_endpoint_;
            client.DecodeDatagram(buffer_.data(), bytes_transfered);
            break;
          }
        }

        Start();
    });
}

}  // namespace upzp::client_com
