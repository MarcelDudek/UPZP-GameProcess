#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#define ASIO_STANDALONE
#endif

#include "client_communication/inc/client_communication.h"
#include "main_process_comm/inc/main_process_comm.h"
#include "game_logic/inc/game_logic.h"
#include "datagram/inc/datagram.h"
#include "sub_process_settings.h"
#include <iostream>
#include <iomanip>
#include <chrono>

/**
 * @brief Main function.
 * @param argc 
 * @param argv 
 * @return err_code
 * @todo Add communication with main process!!!
*/
int main(int argc, char* argv[]) {
  upzp::SubProcessSettings settings(argc, argv);

  // create game logic object
  auto game_logic = std::make_shared<upzp::game_logic::GameLogic>();
  game_logic->NewGame(
      {settings.start_point_longitude, settings.start_point_latitude},
      settings.map_radius, settings.map_name, settings.game_id, settings.points_to_win,
      settings.point_box_spawn_period);

  // create client communication object
  std::unique_ptr<upzp::client_com::ClientCommunication> client_comm;
  std::unique_ptr<upzp::main_process_comm::MainProcessComm> main_process_comm;
  try {
    main_process_comm = std::make_unique<upzp::main_process_comm::MainProcessComm>("127.0.0.1", 3333);
    client_comm = std::make_unique<upzp::client_com::ClientCommunication>(settings.udp_port);
    client_comm->AssignGameLogic(game_logic);
    game_logic->StartGame();  // start game logic thread
    main_process_comm->Start();
    client_comm->Start();  // start client communication thread
  } catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }

  // loop to keep application alive
  while (game_logic->Running()) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5s);
  }

  return 0;
}
