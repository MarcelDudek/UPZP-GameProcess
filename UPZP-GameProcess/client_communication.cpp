#include "client_communication.h"
#include "game_status_generated.h"
#include "datagram.h"
#include <iostream>

namespace upzp::client_com {

  /**
 * @brief Constructor.
 * @param port Port on which communication will be open.
 */
ClientCommunication::ClientCommunication(const unsigned int port)
    : socket_(context_, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
      receive_buffer_(RECEIVE_BUFFER_SIZE), transmit_timer_(context_), game_status_period_(1000) {
  running_ = false;
  sequence_number_ = 1;
}

/**
 * @brief Add client.
 * @param client Client to add.
*/
void ClientCommunication::AddClient(Client client) {
  clients_mutex_.lock();
  clients_.push_back(client);
  clients_mutex_.unlock();
}

/**
 * @brief Add multiple clients.
 * @param client List of clients.
*/
void ClientCommunication::AddClient(std::vector<Client>& clients) {
  clients_mutex_.lock();
  for (auto& client : clients) {
    clients_.push_back(client);
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
      running_ = false;
      });
  }
}

/**
 * @brief Start receiving thread.
*/
void ClientCommunication::StartReceive() {
  socket_.async_receive_from(asio::buffer(receive_buffer_.data(), receive_buffer_.size()),
    remote_endpoint_,
    [this](const asio::error_code& error, std::size_t bytes_transfered) {
      clients_mutex_.lock();
      for (auto& client : clients_) {  // check all clients for coresponding address
        if (remote_endpoint_.address().to_string() == client.Ip_v4() && remote_endpoint_.port() == client.Port()) {
          try {
            client.DecodeDatagram(receive_buffer_.data(), bytes_transfered);
          }
          catch (std::exception& ex) {}
          //std::cout << "Received from " << remote_endpoint_.address().to_string() << ":" << remote_endpoint_.port()
          //  << " " << bytes_transfered << " bytes of data:\n";
          break;
        }
      }
      clients_mutex_.unlock();

      StartReceive();  // start new receive
    });
}

/**
 * @brief Start transmiting thread.
*/
void ClientCommunication::StartTransmit() {
  transmit_timer_.expires_after(game_status_period_);
  transmit_timer_.async_wait([this](const asio::error_code& error) {
    GenerateGameStatus(sequence_number_++);
    clients_mutex_.lock();
    for (auto& client : clients_) {  // send game status to every client
      socket_.async_send_to(asio::buffer(transmit_buffer_.data(), transmit_buffer_.size()), client.remote_endpoint_,
        [this](const asio::error_code& error, std::size_t bytes_transfered) {
          //std::cout << "Transmitted " << bytes_transfered << " bytes of data.\n";
        });
    }
    clients_mutex_.unlock();

    StartTransmit();  // start new transmit
    });
}

/**
 * @brief Gamerate game status (mocker function).
 * @param seq_num Sequence number.
*/
void ClientCommunication::GenerateGameStatus(uint64_t seq_num) {
  flatbuffers::FlatBufferBuilder builder(4096);

  // create players
  auto player_1_name = builder.CreateString("player_1");
  auto player_2_name = builder.CreateString("player_2");
  auto player_3_name = builder.CreateString("player_3");
  auto player_4_name = builder.CreateString("player_4");
  auto player_1_position = Upzp::GameStatus::Position(17.059766, 51.111141);
  auto player_2_position = Upzp::GameStatus::Position(17.064647, 51.114305);
  auto player_3_position = Upzp::GameStatus::Position(17.058302, 51.107071);
  auto player_4_position = Upzp::GameStatus::Position(17.053932, 51.104906);
  auto player_1 = Upzp::GameStatus::CreatePlayer(builder, player_1_name, 0x100, 5200, &player_1_position, Upzp::GameStatus::Vehicle::Vehicle_Car);
  auto player_2 = Upzp::GameStatus::CreatePlayer(builder, player_2_name, 0x200, 2330, &player_2_position, Upzp::GameStatus::Vehicle::Vehicle_Car);
  auto player_3 = Upzp::GameStatus::CreatePlayer(builder, player_3_name, 0x300, 3010, &player_3_position, Upzp::GameStatus::Vehicle::Vehicle_Car);
  auto player_4 = Upzp::GameStatus::CreatePlayer(builder, player_4_name, 0x400, 4444, &player_4_position, Upzp::GameStatus::Vehicle::Vehicle_Car);

  // create teams
  std::vector<flatbuffers::Offset<Upzp::GameStatus::Player>> team_1_players_vec;
  std::vector<flatbuffers::Offset<Upzp::GameStatus::Player>> team_2_players_vec;
  team_1_players_vec.push_back(player_2);
  team_1_players_vec.push_back(player_1);
  team_2_players_vec.push_back(player_3);
  team_2_players_vec.push_back(player_4);
  auto team_1_players = builder.CreateVector(team_1_players_vec);
  auto team_2_players = builder.CreateVector(team_2_players_vec);
  auto team_1 = Upzp::GameStatus::CreateTeam(builder, 7530, team_1_players);
  auto team_2 = Upzp::GameStatus::CreateTeam(builder, 7454, team_2_players);

  // create boxes
  auto box_1_position = Upzp::GameStatus::Position(17.059235, 51.109738);
  auto box_2_position = Upzp::GameStatus::Position(17.057037, 51.113315);
  auto box_3_position = Upzp::GameStatus::Position(17.052791, 51.109543);
  auto box_1 = Upzp::GameStatus::CreatePointBox(builder, &box_1_position, 100);
  auto box_2 = Upzp::GameStatus::CreatePointBox(builder, &box_2_position, 100);
  auto box_3 = Upzp::GameStatus::CreatePointBox(builder, &box_3_position, 100);

  // create game
  std::vector<flatbuffers::Offset<Upzp::GameStatus::PointBox>> point_boxes_vector;
  point_boxes_vector.push_back(box_1);
  point_boxes_vector.push_back(box_2);
  point_boxes_vector.push_back(box_3);
  auto point_boxes = builder.CreateVector(point_boxes_vector);
  std::vector<flatbuffers::Offset<Upzp::GameStatus::Team>> teams_vector;
  teams_vector.push_back(team_1);
  teams_vector.push_back(team_2);
  auto teams = builder.CreateVector(teams_vector);
  auto game = Upzp::GameStatus::CreateGame(builder, seq_num, teams, point_boxes, false);

  builder.Finish(game);
  transmit_buffer_ = std::vector<char>(builder.GetSize());
  Datagram datagram;
  datagram.SetVersion(GAME_STATUS_VER);
  datagram.SetPayload(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
  datagram.SetPayloadChecksum(true);
  transmit_buffer_ = datagram.Get();
}

}  // namespace upzp::client_com
