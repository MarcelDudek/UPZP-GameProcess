cmake_minimum_required(VERSION 3.16)
project(UPZP_GameProcess)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -DASIO_STANDALONE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# MySQL
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/UPZP-GameProcess")
find_package(MySQL REQUIRED)
include_directories(${MYSQL_INCLUDE_DIR})

find_package(Threads REQUIRED)

# INCLUDE FLATBUFFERS AND ASIO
include_directories(
        /usr/include/asio
        /snap/flatbuffers/current/include
        /usr/include/cppconn)

# INCLUDE DIRECTORIES
include_directories(
        UPZP-GameProcess/inc
        UPZP-GameProcess/client_communication/inc
        UPZP-GameProcess/datagram/inc
        UPZP-GameProcess/game_logic/inc
        UPZP-GameProcess/main_process_comm/inc
        client_communication_mocker/inc
        main_process_communication_mocker/inc)

# MAIN EXECUTABLE FOR SUB-PROCESS
add_executable(UPZP_GameProcess
        UPZP-GameProcess/main.cpp
        UPZP-GameProcess/client.cpp
        UPZP-GameProcess/datagram/datagram.cpp
        UPZP-GameProcess/client_communication/client_communication.cpp
        UPZP-GameProcess/client_communication/client_udp.cpp
        UPZP-GameProcess/game_logic/player.cpp
        UPZP-GameProcess/game_logic/team.cpp
        UPZP-GameProcess/game_logic/game.cpp
        UPZP-GameProcess/game_logic/game_logic.cpp
        UPZP-GameProcess/sub_process_settings.cpp UPZP-GameProcess/main_process_comm/main_process_comm.cpp UPZP-GameProcess/main_process_comm/inc/main_process_comm.h UPZP-GameProcess/datagram/datagram_stream.cpp UPZP-GameProcess/datagram/inc/datagram_stream.h)

add_executable(client_communication_mocker
        client_communication_mocker/main.cpp
        UPZP-GameProcess/datagram/datagram.cpp
        client_communication_mocker/udp_communication_mocker.cpp)

add_executable(main_process_communication_mocker
        main_process_communication_mocker/main.cpp
        UPZP-GameProcess/datagram/datagram.cpp
        main_process_communication_mocker/tcp_communication_mocker.cpp)

target_link_libraries(UPZP_GameProcess ${MYSQL_LIB} -lmysqlcppconn)

target_compile_definitions(UPZP_GameProcess INTERFACE ASIO_STANDALONE)
target_compile_definitions(client_communication_mocker INTERFACE ASIO_STANDALONE)
target_compile_definitions(main_process_communication_mocker INTERFACE ASIO_STANDALONE)
