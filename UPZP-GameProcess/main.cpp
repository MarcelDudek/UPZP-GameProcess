#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include "client_communication/inc/client_communication.h"
#include "datagram/inc/datagram.h"
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
void LoadClient(upzp::client_com::ClientCommunication& comm) {
  std::cout << "Add new client (example input 127.0.0.1:9000): ";

  std::string client_address;
  std::cin >> client_address;

  std::string ip = client_address.substr(0, client_address.find_first_of(':'));
  std::string port_str = client_address.substr(client_address.find_first_of(':') + 1);
  int port = std::stoi(port_str);
  static uint32_t id = 1;
  upzp::Client client("name", id++, upzp::VehicleType::CAR, ip, port);
  comm.AddClient(client);
}

/**
 * @brief Main function.
 * @param argc 
 * @param argv 
 * @return err_code
*/
int main(int argc, char* argv[]) {
  std::unique_ptr<upzp::client_com::ClientCommunication> client_comm;
  try {
    client_comm = std::make_unique<upzp::client_com::ClientCommunication>(LoadClientCommPortArg(argc, argv));
    client_comm->Start();
  } catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }

  // loop to keep application alive
  while (true) {
    LoadClient(*client_comm.get());
    //using namespace std::chrono_literals;
    //std::this_thread::sleep_for(5min);
  }
}
