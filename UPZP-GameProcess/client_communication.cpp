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
      buffer_(10) {
}

/**
 * @brief Start the client communication.
 */
void ClientCommunication::Start() {
  socket_.async_receive_from(asio::buffer(buffer_.data(), buffer_.size()),
                             remote_endpoint_,
    [this](const asio::error_code& error, std::size_t bytes_transfered) {
        std::cout << "Received from " << remote_endpoint_.address().to_string()
          << " " << bytes_transfered << " bytes of data:\n";
        for (size_t i = 0; i < bytes_transfered; i++)
          std::cout << buffer_[i];
        std::cout << std::endl;
        Start();
    });
}

}  // namespace upzp::client_com
