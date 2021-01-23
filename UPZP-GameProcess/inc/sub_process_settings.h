#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_SUB_PROCESS_SETTINGS_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_SUB_PROCESS_SETTINGS_H_

#include <cstdint>
#include <string>

namespace upzp {

struct SubProcessSettings {
  uint16_t udp_port = 8850;  /**< Port for UDP communication with clients. */
  uint16_t tcp_port = 8860;  /**< Port for TCP communication with main process. */
  double start_point_latitude = 17.034299;  /**< Map's center point latitude in degrees. */
  double start_point_longitude = 51.109385;  /**< Map's center point longitude in degrees. */
  double map_radius = 1000.0;  /**< Map's radius in meters. */
  std::string map_name;  /**< Map's name. */
  uint32_t game_id = 0x0;  /**< Game's unique ID. */
  int64_t points_to_win = 1000;  /**< How many points does team need to win the game. */
  uint16_t point_box_spawn_period = 10;  /**< Point box spawn period in seconds. */

  SubProcessSettings();
  SubProcessSettings(int argc, char* argv[]);
  void Load(int argc, char* argv[]);
};

}  // namespace uzpz

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_SUB_PROCESS_SETTINGS_H_
