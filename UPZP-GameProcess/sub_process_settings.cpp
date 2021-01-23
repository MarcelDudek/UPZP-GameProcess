#include <cstring>
#include <iostream>
#include "sub_process_settings.h"

namespace upzp {

/**
 * @brief Default constructor.
 */
SubProcessSettings::SubProcessSettings() : map_name("WROCLAW") {
}

/**
 * Construct the settings structure with
 * program arguments. This constructor is
 * using the Load method.
 *
 * @brief Constructor.
 * @param argc
 * @param argv
 */
SubProcessSettings::SubProcessSettings(int argc, char **argv) : SubProcessSettings() {
  Load(argc, argv);
}

/**
 * Load sub-process settings from program arguments.
 *
 * @brief Load settings from arguments.
 * @param argc
 * @param argv
 */
void SubProcessSettings::Load(int argc, char **argv) {
  for (int i = 0; i < argc - 1; i++) {
    if (!strcmp("--udp", argv[i])) {
      try {
        udp_port = std::stoi(argv[i + 1]);
      } catch (std::exception &ex) {
        std::cerr << "--udp:" << ex.what() << std::endl;
      }
    } else if (!strcmp("--tcp", argv[i])) {
      try {
        tcp_port = std::stoi(argv[i + 1]);
      } catch (std::exception &ex) {
        std::cerr << "--tcp:" << ex.what() << std::endl;
      }
    } else if (!strcmp("--lat_start", argv[i])) {
      try {
        start_point_latitude = std::stod(argv[i + 1]);
      } catch (std::exception &ex) {
        std::cerr << "--lat_start:" << ex.what() << std::endl;
      }
    } else if (!strcmp("--long_start", argv[i])) {
      try {
        start_point_longitude = std::stod(argv[i + 1]);
      } catch (std::exception &ex) {
        std::cerr << "--long_start:" << ex.what() << std::endl;
      }
    } else if (!strcmp("--radius", argv[i])) {
      try {
        map_radius = std::stod(argv[i + 1]);
      } catch (std::exception &ex) {
        std::cerr << "--radius:" << ex.what() << std::endl;
      }
    } else if (!strcmp("--map", argv[i])) {
      map_name = argv[i + 1];
    } else if (!strcmp("--id", argv[i])) {
      try {
        game_id = std::stoi(argv[i + 1]);
      } catch (std::exception &ex) {
        std::cerr << "--id:" << ex.what() << std::endl;
      }
    } else if (!strcmp("--win_points", argv[i])) {
      try {
        points_to_win = std::stoi(argv[i + 1]);
      } catch (std::exception &ex) {
        std::cerr << "--win_points:" << ex.what() << std::endl;
      }
    } else if (!strcmp("--spawn_period", argv[i])) {
      try {
        point_box_spawn_period = std::stoi(argv[i + 1]);
      } catch (std::exception &ex) {
        std::cerr << "--spawn_period:" << ex.what() << std::endl;
      }
    }
  }
}

}  // namespace upzp
