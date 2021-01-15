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
  static constexpr uint32_t POINT_BOX_SPAWN_PERIOD = 10;
  const std::chrono::milliseconds tick_duration_;
  const std::chrono::seconds point_box_spawn_period_;

  std::unique_ptr<Game> game_;
  bool game_started_ = false;
  std::thread game_thread_;
  std::mutex mutex_;
  uint64_t serialization_seq_num_ = 0;
  std::chrono::time_point<std::chrono::system_clock> last_point_box_spawn_;

  void Tick();
  bool GameFinished();

 public:
  GameLogic();
  void NewGame(Maps map);
  void AddPlayer(Client client, bool to_red_team);
  void StartGame();
  bool Running();
  void GetGameStatus(flatbuffers::FlatBufferBuilder&);
  void SetPlayerMovement(PlayerInput);
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_
