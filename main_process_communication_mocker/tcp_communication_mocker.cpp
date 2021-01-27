#include "tcp_communication_mocker.h"
#include "game_generated.h"

namespace upzp::mocker {

/**
 * @brief Mock game.fbs with game player base.
 * @param team_1_player_count How many players in team red?
 * @param team_2_player_count How many players in team blue?
 * @return Datagram with message.
 */
Datagram MockPlayerListDatagram(std::size_t team_1_player_count, std::size_t team_2_player_count) {
  flatbuffers::FlatBufferBuilder builder(1024);

  std::vector<flatbuffers::Offset<mainServer::schemas::FGame::FClient>> players_1_vec;
  std::vector<flatbuffers::Offset<mainServer::schemas::FGame::FClient>> players_2_vec;

  // team red players
  for (std::size_t i = 0; i < team_1_player_count; i++) {
    auto name = builder.CreateString("player_" + std::to_string(i + 1) + "_team_red");
    uint32_t id = 1000 + i;
    auto ip_address = builder.CreateString("127.0.0.1");
    uint16_t port = 5100 + i;
    static uint8_t vehicle = 1;
    if(++vehicle > 3)
      vehicle = 1;
    flatbuffers::Offset<mainServer::schemas::FGame::FVehicle> f_vehicle;
    switch (vehicle) {
      case 1:
        f_vehicle = mainServer::schemas::FGame::CreateFVehicle(
            builder,mainServer::schemas::FGame::FVehicleType::FVehicleType_Pedestrian);
        break;
      case 2:
        f_vehicle = mainServer::schemas::FGame::CreateFVehicle(
            builder,mainServer::schemas::FGame::FVehicleType::FVehicleType_Cyclist);
        break;
      case 3:
        f_vehicle = mainServer::schemas::FGame::CreateFVehicle(
            builder,mainServer::schemas::FGame::FVehicleType::FVehicleType_Car);
        break;
      default:
        vehicle = 1;
        f_vehicle = mainServer::schemas::FGame::CreateFVehicle(
            builder,mainServer::schemas::FGame::FVehicleType::FVehicleType_Pedestrian);
        break;
    }
    auto player = mainServer::schemas::FGame::CreateFClient(builder, name, id, ip_address, port, f_vehicle);
    players_1_vec.push_back(player);
  }

  // team blue players
  for (std::size_t i = 0; i < team_2_player_count; i++) {
    auto name = builder.CreateString("player_" + std::to_string(i + 1) + "_team_blue");
    uint32_t id = 2000 + i;
    auto ip_address = builder.CreateString("127.0.0.1");
    uint16_t port = 5500 + i;
    static uint8_t vehicle = 1;
    if(++vehicle > 3)
      vehicle = 1;
    flatbuffers::Offset<mainServer::schemas::FGame::FVehicle> f_vehicle;
    switch (vehicle) {
      case 1:
        f_vehicle = mainServer::schemas::FGame::CreateFVehicle(
            builder,mainServer::schemas::FGame::FVehicleType::FVehicleType_Pedestrian);
        break;
      case 2:
        f_vehicle = mainServer::schemas::FGame::CreateFVehicle(
            builder,mainServer::schemas::FGame::FVehicleType::FVehicleType_Cyclist);
        break;
      case 3:
        f_vehicle = mainServer::schemas::FGame::CreateFVehicle(
            builder,mainServer::schemas::FGame::FVehicleType::FVehicleType_Car);
        break;
      default:
        vehicle = 1;
        f_vehicle = mainServer::schemas::FGame::CreateFVehicle(
            builder,mainServer::schemas::FGame::FVehicleType::FVehicleType_Pedestrian);
        break;
    }
    auto player = mainServer::schemas::FGame::CreateFClient(builder, name, id, ip_address, port, f_vehicle);
    players_2_vec.push_back(player);
  }

  auto team_1_players = builder.CreateVector(players_1_vec);
  auto team_2_players = builder.CreateVector(players_2_vec);
  std::vector<flatbuffers::Offset<mainServer::schemas::FGame::FTeam>> teams_vec;
  auto team_red = mainServer::schemas::FGame::CreateFTeam(builder, team_1_players);
  auto team_blue = mainServer::schemas::FGame::CreateFTeam(builder, team_2_players);
  teams_vec.push_back(team_red);
  teams_vec.push_back(team_blue);
  auto teams = builder.CreateVector(teams_vec);
  auto game = mainServer::schemas::FGame::CreateFGame(builder, 55555, teams);
  builder.Finish(game);

  Datagram ret_val;
  ret_val.SetVersion(10);
  ret_val.SetPayloadChecksum(true);
  ret_val.SetPayload(reinterpret_cast<const char*>(builder.GetBufferPointer()),
                      builder.GetSize());
  return ret_val;
}

}