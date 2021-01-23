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
#include "mysql_connection.h"

namespace upzp::game_logic {

class GameLogic {
 private:
  static constexpr uint32_t TICK_RATE = 20;  /**< Tick rate at which the game will be refreshed */
  const std::chrono::milliseconds tick_duration_;

  std::unique_ptr<Game> game_;
  uint32_t game_id_;
  std::string map_name_;
  std::chrono::seconds point_box_spawn_period_;

  // start and end point of the game (for statistics)
  decltype(std::chrono::system_clock::now()) start_point_, finish_point_;

  bool game_started_ = false;
  std::thread game_thread_;
  std::mutex mutex_;
  uint64_t serialization_seq_num_ = 0;
  std::chrono::time_point<std::chrono::system_clock> last_point_box_spawn_;

  void Tick();
  bool GameFinished();
  void SendStatisticsToDatabase();
  void CreatePlayersTableStatement(sql::PreparedStatement** prepared_stmt, sql::Connection* conn);

 public:
  GameLogic();
  void NewGame(Coordinates start_point, double radius, std::string map_name, uint32_t game_id,
               uint64_t point_to_win, uint16_t point_box_spawn_period);
  void AddPlayer(Client client, bool to_red_team);
  void StartGame();
  bool Running();
  void GetGameStatus(flatbuffers::FlatBufferBuilder&);
  void SetPlayerMovement(PlayerInput);
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_
