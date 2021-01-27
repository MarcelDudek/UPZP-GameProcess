#include "inc/game_logic.h"
#include <ctime>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include <utility>


namespace upzp::game_logic {

/**
 * @brief Constructor.
 */
GameLogic::GameLogic() : tick_duration_(1000 / TICK_RATE),
  point_box_spawn_period_(10), last_point_box_spawn_(std::chrono::system_clock::now()) {
}

/**
 * @brief Destructor.
 */
GameLogic::~GameLogic() {
  StopGame();
}

/**
 * @brief Start a new game.
 * @param start_point Start point of the map
 * @param radius Radius of the map in meters.
 * @param map_name Name of the map.
 * @param game_id ID of the game that will be send to database.
 * @param point_to_win How many points does the team need to win the game?
 * @param point_box_spawn_period Point boxes' spawn period in seconds.
 */
void GameLogic::NewGame(Coordinates start_point, double radius, std::string map_name,
                        uint32_t game_id, uint64_t point_to_win, uint16_t point_box_spawn_period) {
  point_box_spawn_period_ = std::chrono::seconds(point_box_spawn_period);
  game_ = std::make_unique<Game>(point_to_win, start_point, radius);
  game_id_ = game_id;
  map_name_ = std::move(map_name);
}

/**
 * @brief Add new player to the game.
 * @param client New player.
 * @param to_red_team Should be added to red team (otherwise will be added to blue)?
 */
void GameLogic::AddPlayer(const Client& client, bool to_red_team) {
  if (game_) {
    mutex_.lock();
    game_->AddPlayer(client.Name(), client.Id(), client.Vehicle(), to_red_team);
    mutex_.unlock();
  }
}

/**
 * @brief Start the game thread.
 */
void GameLogic::StartGame() {
  if (!game_started_ && game_) {
    start_point_ = std::chrono::system_clock::now();
    game_started_ = true;
    terminate_ = false;
    game_thread_ = std::thread([this](){
      do {  // game loop
        Tick();
        std::this_thread::sleep_for(tick_duration_);
      } while (!GameFinished() && !terminate_);
      // when the game has finished
      if (terminate_) {
        game_started_ = false;
        return;
      }
      finish_point_ = std::chrono::system_clock::now();
      std::cout << "Game finished.\n";
      SendStatisticsToDatabase();
      std::cout << "Game thread terminated.\n";
      game_started_ = false;
    });
  }
}

/**
 * @brief Game tick.
 */
void GameLogic::Tick() {
  mutex_.lock();
  // check for point box spawn
  auto now = std::chrono::system_clock::now();
  if (now - last_point_box_spawn_ > point_box_spawn_period_) {
    game_->GeneratePointBox();
    last_point_box_spawn_ = now;
  }

  game_->CalculateMovement(
      std::chrono::duration_cast<std::chrono::duration<double>>(tick_duration_));
  serialization_seq_num_++;
  mutex_.unlock();
}

/**
 * Check if the game has finished which means
 * that one of the teams has won the game.
 *
 * @brief Check if the game has finished.
 * @return True if game finished.
 */
bool GameLogic::GameFinished() {
  mutex_.lock();
  bool ret_val = game_->BlueTeamWon() || game_->RedTeamWon();
  mutex_.unlock();
  return ret_val;
}

/**
 * Get game status serialization using flatbuffers.
 *
 * @brief Get game status.
 * @param builder Flatbuffers builder.
 */
void GameLogic::GetGameStatus(flatbuffers::FlatBufferBuilder& builder) {
  if (game_) {
    mutex_.lock();
    game_->GenerateFlatbuffers(builder, serialization_seq_num_);
    mutex_.unlock();
  }
}

/**
 * Set player's movement using received input.
 *
 * @brief Set player's movement.
 * @param input Player's input.
 */
void GameLogic::SetPlayerMovement(PlayerInput input) {
  if (game_) {
    mutex_.lock();
    game_->SetPlayerInput(input);
    mutex_.unlock();
  }
}

/**
 * Is the game thread running?
 *
 * @brief Check if the game is running.
 * @return True for game thread running.
 */
bool GameLogic::Running() {
  bool ret_val;
  mutex_.lock();
  ret_val =  game_started_;
  mutex_.unlock();
  return ret_val;
}

/**
 * Send statistics of the game and players to MYSQL
 * database.
 *
 * @brief Send statistics of the game to MYSQL database.
 */
void GameLogic::SendStatisticsToDatabase() {
  // serialize time points into datetime strings for query
  auto game_start_point = std::chrono::system_clock::to_time_t(start_point_);
  auto game_finish_point = std::chrono::system_clock::to_time_t(finish_point_);
  char game_start_str[128];
  char game_finish_str[128];
  auto time_info = localtime(&game_start_point);
  std::strftime(game_start_str, 128, "%F %T", time_info);
  time_info = localtime(&game_finish_point);
  std::strftime(game_finish_str, 128, "%F %T", time_info);

  try {
    sql::Driver *driver;
    sql::Connection *conn;
    sql::ConnectOptionsMap conn_properties;
    conn_properties["hostName"] = DB_HOST_NAME;
    conn_properties["userName"] = DB_USER_NAME;
    conn_properties["password"] = DB_PASSWORD;
    conn_properties["schema"] = DB_SCHEMA;
    conn_properties["sslCA"] = DB_SSL_CA;
    conn_properties["sslCert"] = DB_SSL_CRT;
    conn_properties["sslKey"] = DB_SSL_KEY;

    // connect to database
    driver = get_driver_instance();
    conn = driver->connect(conn_properties);
    conn->setSchema(DB_SCHEMA);

    // create game table quote
    sql::PreparedStatement* prepared_stmt = conn->prepareStatement(
        "INSERT INTO stat_map_game(game_id, map, start_time, end_time,"
        "team_red_points, team_blue_points, team_won)"
        "VALUES (?, ?, ?, ?, ?, ?, ?)");
    prepared_stmt->setInt(1,game_id_);
    prepared_stmt->setString(2,map_name_);
    prepared_stmt->setDateTime(3, game_start_str);
    prepared_stmt->setDateTime(4, game_finish_str);
    prepared_stmt->setInt(5, game_->RedTeamScore());
    prepared_stmt->setInt(6, game_->BlueTeamScore());
    game_->RedTeamWon() ? prepared_stmt->setString(7, "red") :
    prepared_stmt->setString(7, "blue");
    prepared_stmt->execute();
    delete prepared_stmt;

    // create player table quote
    game_->CreatePlayersTableStatement(&prepared_stmt, conn, game_id_, map_name_);
    prepared_stmt->execute();
    delete prepared_stmt;

    delete conn;
    std::cout << "Game statistics inserted into the database.\n";
  } catch (sql::SQLException &e) {
    using std::cout;
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line "
    << __LINE__ << std::endl;
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
  }
}

/**
 * @brief Stop the game thread.
 */
void GameLogic::StopGame() {
  terminate_ = true;
  if (game_thread_.joinable())
    game_thread_.join();
}

}  // namespace upzp::game_logic
