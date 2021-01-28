#include "inc/main_process_comm.h"
#include <iostream>
#include <utility>
#include "game_generated.h"
#include "game_ended_generated.h"
#include "game_id_generated.h"

namespace upzp::main_process_comm {

/**
 * @brief Constructor.
 * @param port Port for TCP communication.
 */
MainProcessComm::MainProcessComm(std::string address, uint16_t port, uint32_t game_id) : context_(),
socket_(context_), ip_v4_(std::move(address)), port_(port), game_id_(game_id), buffer_(1024) {
}

/**
 * @brief Destructor.
 */
MainProcessComm::~MainProcessComm() {
  Stop();
}

/**
 * @brief Start main process communication in thread.
 */
void MainProcessComm::Start() {
  if (!running_) {
    running_ = true;
    run_thread_ = std::thread([this]() {
      try {
        Connect();
        context_.run();
      } catch (std::exception& ex) {
        std::cout << "Main process connection error: " << ex.what() << std::endl;
      }
      std::cout << "Main process connection terminated.\n";
      running_ = false;
    });
  }
}

/**
 * @brief Connect to TCP connection.
 */
void MainProcessComm::Connect() {
  asio::error_code error_;
  asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip_v4_, error_), port_);
  if (error_)
    throw asio::system_error(error_);
  socket_.connect(endpoint, error_);
  if (error_)
    throw asio::system_error(error_);
  std::cout << "Connected to main process\n";
  TransmitGameId();
  StartReceive();
}

/**
 * @brief Load clients from received datagram.
 */
void MainProcessComm::LoadClients() {
  if (datagram_stream_.Version() != GAME_CLIENTS_VERSION)
    return;
  auto payload = datagram_stream_.Payload();
  auto game = mainServer::schemas::FGame::GetFGame(payload.data());
  auto teams = game->teams();
  if (teams->size() < 2)
    return;

  // add clients
  std::size_t clients_count = 0;
  for (std::size_t i = 0; i < 2; i++) {
    auto clients = (*teams)[i]->clients();
    clients_count += clients->size();
    for (auto client : *clients) {
      VehicleType vehicle_type;
      using mainServer::schemas::FGame::FVehicleType;
      switch (client->vehicle()->vehicleType()) {
        case FVehicleType::FVehicleType_Pedestrian:
          vehicle_type = VehicleType::PEDESTRIAN;
          break;
        case FVehicleType::FVehicleType_Cyclist:
          vehicle_type = VehicleType::CYCLIST;
          break;
        case FVehicleType::FVehicleType_Car:
          vehicle_type = VehicleType::CAR;
          break;
        default:
          vehicle_type = VehicleType::PEDESTRIAN;
          break;
      }
      Client _client(client->name()->str(), client->id(), vehicle_type,
                     client->ipAddress()->str(), client->port());
      if (client_comm_)
        client_comm_->AddClient(_client);
      if (game_logic_)
        game_logic_->AddPlayer(_client, i == 0);
    }
  }
  std::cout << "Added " + std::to_string(clients_count) + " clients.\n";
}

/**
 * Assign client communication object to which
 * information about clients will be send.
 *
 * @brief Assign client communication object.
 * @param client_comm Client communication shared pointer.
 */
void MainProcessComm::AssignClientCommunication(
    std::shared_ptr<client_com::ClientCommunication> client_comm) {
  client_comm_ = std::move(client_comm);
}

/**
 * Assign game_logic object to which
 * information about clients will be send.
 *
 * @brief Assign game logic object.
 * @param game_logic Client communication shared pointer.
 */
void MainProcessComm::AssignGameLogic(std::shared_ptr<game_logic::GameLogic> game_logic) {
  game_logic_ = std::move(game_logic);
}

/**
 * @brief Send information about game finished.
 */
void MainProcessComm::SendGameFinished() {
  flatbuffers::FlatBufferBuilder builder(256);
  auto game_finished = mainServer::schemas::FGameEnded::CreateFGameEnded(builder, true);
  builder.Finish(game_finished);
  Datagram datagram;
  datagram.SetVersion(11);
  datagram.SetPayloadChecksum(false);
  datagram.SetPayload(reinterpret_cast<const char*>(builder.GetBufferPointer()),
                      builder.GetSize());
  auto buffer = datagram.Get();
  socket_.async_write_some(
      asio::buffer(buffer),
      [this](const asio::error_code& error, std::size_t bytes_transferred){
        if (error)
          std::cout << asio::system_error(error).what() << std::endl;
        else
          std::cout << "Transmitted game finished state to main process.\n";
      });
}

/**
 * @brief Start receive.
 */
void MainProcessComm::StartReceive() {
  socket_.async_read_some(
      asio::buffer(buffer_),
      [this](const asio::error_code& error, std::size_t bytes_transferred) {
        if (error != asio::error::eof && error) {  // if there is a connection error
          std::cout << asio::system_error(error).what() << std::endl;
          return;
        }

        if (error == asio::error::eof)  // if connection ended on main process side
          return;

        // read data
        if (datagram_stream_.AddData(buffer_.data(), bytes_transferred)) {
          LoadClients();
          while (datagram_stream_.FlushData())
            LoadClients();
        }

        StartReceive();
  });
}

/**
 * @brief Stop communication.
 */
void MainProcessComm::Stop() {
  if (running_) {
    socket_.cancel();
    socket_.close();
    context_.stop();
  }
  if (run_thread_.joinable())
    run_thread_.join();
}

/**
 * Transmit game ID to the main process in order
 * to verify the communication.
 *
 * @brief Transmit game ID.
 */
void MainProcessComm::TransmitGameId() {
  flatbuffers::FlatBufferBuilder builder(256);
  auto f_game_id = mainServer::schemas::FGameId::CreateFGameId(builder, game_id_);
  builder.Finish(f_game_id);
  Datagram datagram;
  datagram.SetVersion(12);
  datagram.SetPayloadChecksum(false);
  datagram.SetPayload(reinterpret_cast<const char*>(builder.GetBufferPointer()),
                      builder.GetSize());
  auto buffer = datagram.Get();
  socket_.async_write_some(
      asio::buffer(buffer),
      [this](const asio::error_code& error, std::size_t bytes_transferred) {
        if (error)
          std::cout << asio::system_error(error).what() << std::endl;
        else
          std::cout << "Transmitted game ID to the main process.\n";
      });
}

}  // upzp::main_process_comm
