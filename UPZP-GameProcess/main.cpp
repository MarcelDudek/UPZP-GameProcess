#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include "client_communication/inc/client_communication.h"
#include "game_logic/inc/game_logic.h"
#include "datagram/inc/datagram.h"
#include "maps.h"
#include <asio.hpp>
#include <iostream>
#include <iomanip>

/**
 * @brief Load client communication UDP port from arguments.
 * @param argc 
 * @param argv 
 * @return Client communication port.
*/
int LoadClientCommPortArg(int argc, char* argv[]) {
  constexpr int DEFAULT_PORT = 8550;
  int retval = DEFAULT_PORT;  // default value

  for (int i = 0; i < argc - 1; i++) {
    if (!strcmp("--udp-port", argv[i])) {
      try {
        retval = std::stoi(argv[i + 1]);
      }
      catch (std::exception& ex) {
        std::cerr << "During loading of client communication port: " << ex.what() << std::endl;
        retval = DEFAULT_PORT;
      }
    }
  }

  return retval;
}

/**
 * @brief Load client from std input.
 * @param comm Reference to client communication, where loaded client will be added.
*/
void LoadClient(upzp::client_com::ClientCommunication& comm, upzp::game_logic::GameLogic& game_logic) {
  // load client's IPv4 address
  std::string client_address;
  std::cout << "Add new client (example input 127.0.0.1:9000): ";
  std::cin >> client_address;

  // get address and port from a string
  std::string ip = client_address.substr(0, client_address.find_first_of(':'));
  std::string port_str = client_address.substr(client_address.find_first_of(':') + 1);
  int port = std::stoi(port_str);

  // load client name
  std::string name;
  std::cout << "Client name: ";
  std::cin >> name;

  // load client id
  uint32_t id;
  std::cout << "Client ID: ";
  std::cin >> id;

  // load to which team
  bool to_red_team;
  std::cout << "Add to red team (1/0): ";
  std::cin >> to_red_team;

  upzp::Client client(name, id++, upzp::VehicleType::CAR, ip, port);
  comm.AddClient(client);
  game_logic.AddPlayer(client, to_red_team);
}

/**
 * @brief Main function.
 * @param argc 
 * @param argv 
 * @return err_code
 * @todo Add other arguments loading.
*/
int main(int argc, char* argv[]) {
  auto game_logic = std::make_shared<upzp::game_logic::GameLogic>();
  game_logic->NewGame(
      {upzp::MapsLongitude(upzp::Maps::WROCLAW), upzp::MapsLatitude(upzp::Maps::WROCLAW)},
      2000.0, "WROCLAW", 0x01, 500);
  std::unique_ptr<upzp::client_com::ClientCommunication> client_comm;
  try {
    client_comm = std::make_unique<upzp::client_com::ClientCommunication>(LoadClientCommPortArg(argc, argv));
    client_comm->AssignGameLogic(game_logic);
    game_logic->StartGame();
    client_comm->Start();
  } catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }

  // loop to keep application alive
  while (game_logic->Running()) {
    LoadClient(*client_comm, *game_logic);
    //using namespace std::chrono_literals;
    //std::this_thread::sleep_for(5min);
  }
  // TODO add exit logic
}
