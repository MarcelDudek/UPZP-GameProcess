#include <mainproc_communication.h>
#include <iostream>
#include <string>
#include <sstream>
#include "flatbuffers/flatbuffers.h"
#include "../inc/connection_generated.h"

#pragma comment (lib, "ws2_32.lib")

using namespace std;
namespace upzp::mainproc_com {
    MainProcCommunication::MainProcCommunication(unsigned int port, string ipAddress)
            : socket_(context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
                receive_buffer_(RECEIVE_BUFFER_SIZE){
        cout << "MainProcCommunication works\n";
        this->port=port;
        this->ipAddress=ipAddress;
        running_ = false;
    }

    void MainProcCommunication::Start() {
        if (!running_) {
            running_ = true;
            StartTransmit();
            StartReceive();
            run_thread_ = std::thread([this]() {
                context_.run();
                running_ = false;
            });
        }
    }

    void MainProcCommunication::StartReceive() {

        socket_.async_receive(asio::buffer(receive_buffer_.data(), receive_buffer_.size()),
                              [this](const asio::error_code& error, std::size_t bytes_transfered){
                                  try {

                                  }
                                  catch (std::exception& ex) {}try {

                              }
                              catch (std::exception& ex) {}
                              StartReceive();
        });
    }

    void MainProcCommunication::StartTransmit() {
        GenerateInfo();
        socket_.async_send(asio::buffer(transmit_buffer_.data(), transmit_buffer_.size()),
                              [this](const asio::error_code& error, std::size_t bytes_transfered) {
                                  StartTransmit();
        });

    }
    void MainProcCommunication::GenerateInfo() {
        flatbuffers::FlatBufferBuilder builder(4096);
        auto player_1_address = builder.CreateString("127.0.1.1");
        auto player_2_address = builder.CreateString("127.0.2.1");
        auto player_3_address = builder.CreateString("127.0.3.1");
        auto player_4_address = builder.CreateString("127.0.4.1");
        // create players connection

        auto player_1_connection = Upzp::Connection::CreateFConnection(builder, 0x100, player_1_address, 51000, true);
        auto player_2_connection = Upzp::Connection::CreateFConnection(builder, 0x200, player_2_address, 51001, true);
        auto player_3_connection = Upzp::Connection::CreateFConnection(builder, 0x300, player_3_address, 51002, true);
        auto player_4_connection = Upzp::Connection::CreateFConnection(builder, 0x400, player_4_address, 51003, true);

        std::vector<flatbuffers::Offset<Upzp::Connection::FConnection>> connections_vec;
        connections_vec.push_back(player_1_connection);
        connections_vec.push_back(player_2_connection);
        connections_vec.push_back(player_3_connection);
        connections_vec.push_back(player_4_connection);

        auto connections = builder.CreateVector(connections_vec);
        auto players = Upzp::Connection::CreateFPlayers(builder, connections);

        builder.Finish(players);
        transmit_buffer_ = std::vector<char>(builder.GetSize());

    }
}