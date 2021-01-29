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
  std::shared_ptr<upzp::client_com::ClientCommunication> client_comm;
  std::unique_ptr<upzp::main_process_comm::MainProcessComm> main_process_comm;
  try {
    client_comm = std::make_unique<upzp::client_com::ClientCommunication>(settings.udp_port);
    client_comm->AssignGameLogic(game_logic);
    main_process_comm =
        std::make_unique<upzp::main_process_comm::MainProcessComm>(
            "127.0.0.1", settings.tcp_port, settings.game_id);
    main_process_comm->AssignClientCommunication(client_comm);
    main_process_comm->AssignGameLogic(game_logic);
    game_logic->StartGame();  // start game logic thread
    main_process_comm->Start();  // start main process communication thread
    client_comm->Start();  // start client communication thread
  } catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
    return -1;
  }

  // loop to keep application alive
  while (game_logic->Running()) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5s);
  }

  main_process_comm->SendGameFinished();
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(500ms);
  main_process_comm->Stop();
  client_comm->Stop();

  return 0;
}
