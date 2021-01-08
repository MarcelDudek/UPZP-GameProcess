#ifndef UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_
#define UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_

#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include "game.h"
#include "maps.h"
#include "client.h"

namespace upzp::game_logic {

class GameLogic {
 private:
  static constexpr uint32_t TICK_RATE = 20;  /**< Tick rate at which the game will be refreshed */
  const std::chrono::milliseconds tick_duration_;

  std::unique_ptr<Game> game_;
  bool game_started_ = false;
  std::thread game_thread_;
  std::mutex mutex_;

  void Tick();

 public:
  GameLogic();
  void NewGame(Maps map);
  void AddPlayer(Client client, bool to_red_team);
  void StartGame();
};

}  // namespace upzp::game_logic

#endif //UPZP_GAMEPROCESS_UPZP_GAMEPROCESS_GAME_LOGIC_GAME_LOGIC_H_
