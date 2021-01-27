#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_

#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include "game.h"
#include "maps.h"
#include "client.h"
#include "player_input.h"
#include "game_status_generated.h"

namespace upzp::game_logic {

class GameLogic {
 private:
  static constexpr uint32_t TICK_RATE = 20;  /**< Tick rate at which the game will be refreshed */
  static constexpr auto DB_HOST_NAME = "35.246.226.222";
  static constexpr auto DB_USER_NAME = "dev";
  static constexpr auto DB_PASSWORD = "pz2021project";
  static constexpr auto DB_SCHEMA = "Website";
  static constexpr auto DB_SSL_CA = "./crt/server-ca.pem";
  static constexpr auto DB_SSL_CRT = "./crt/client-cert.pem";
  static constexpr auto DB_SSL_KEY = "./crt/client-key.pem";
  const std::chrono::milliseconds tick_duration_;

  std::unique_ptr<Game> game_;
  uint32_t game_id_ = 0;
  std::string map_name_;
  std::chrono::seconds point_box_spawn_period_;

  // start and end point of the game (for statistics)
  decltype(std::chrono::system_clock::now()) start_point_, finish_point_;

  bool game_started_ = false;
  bool terminate_ = false;
  std::thread game_thread_;
  std::mutex mutex_;
  uint64_t serialization_seq_num_ = 0;
  std::chrono::time_point<std::chrono::system_clock> last_point_box_spawn_;

  void Tick();
  bool GameFinished();
  void SendStatisticsToDatabase();

 public:
  GameLogic();
  virtual ~GameLogic();
  void NewGame(Coordinates start_point, double radius, std::string map_name, uint32_t game_id,
               uint64_t point_to_win, uint16_t point_box_spawn_period);
  void AddPlayer(const Client& client, bool to_red_team);
  void StartGame();
  void StopGame();
  bool Running();
  void GetGameStatus(flatbuffers::FlatBufferBuilder&);
  void SetPlayerMovement(PlayerInput);
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_
