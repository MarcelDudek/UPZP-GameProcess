#ifndef UPZP_GAMEPROCESS_MAIN_PROCESS_COMMUNICATION_MOCKER_INC_TCP_COMMUNICATION_MOCKER_H_
#define UPZP_GAMEPROCESS_MAIN_PROCESS_COMMUNICATION_MOCKER_INC_TCP_COMMUNICATION_MOCKER_H_

#include "datagram.h"
#include <string>

namespace upzp::mocker {

Datagram MockPlayerListDatagram(std::size_t team_1_player_count, std::size_t team_2_player_count);

}

#endif //UPZP_GAMEPROCESS_MAIN_PROCESS_COMMUNICATION_MOCKER_INC_TCP_COMMUNICATION_MOCKER_H_
