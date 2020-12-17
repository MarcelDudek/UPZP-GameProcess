cmake_minimum_required(VERSION 3.17)
project(UPZP_GameProcess)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -DASIO_STANDALONE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(Threads REQUIRED)

# INCLUDE FLATBUFFERS AND ASIO
include_directories(
        /usr/include/asio
        /snap/flatbuffers/current/include)

# INCLUDE DIRECTORIES
include_directories(
        UPZP-GameProcess/inc
        UPZP-GameProcess/client_communication/inc
        UPZP-GameProcess/datagram/inc
        client_communication_mocker/inc)

# MAIN EXECUTABLE FOR SUB-PROCESS
add_executable(UPZP_GameProcess
        UPZP-GameProcess/main.cpp
        UPZP-GameProcess/client.cpp
        UPZP-GameProcess/datagram/datagram.cpp
        UPZP-GameProcess/client_communication/client_communication.cpp
        UPZP-GameProcess/client_communication/client_udp.cpp)

add_executable(client_communication_mocker
        client_communication_mocker/main.cpp
        UPZP-GameProcess/datagram/datagram.cpp
        client_communication_mocker/udp_communication_mocker.cpp)

target_compile_definitions(UPZP_GameProcess INTERFACE ASIO_STANDALONE)
target_compile_definitions(client_communication_mocker INTERFACE ASIO_STANDALONE)