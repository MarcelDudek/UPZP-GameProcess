#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include "client_communication.h"
#include "datagram.h"
#include <asio.hpp>
#include <iostream>

int main() {
  upzp::Datagram d, dp;
  const char data1[] = {0xda, 0xab, 0x05, 0x00, 0x0a, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x6f, 0x2b, 0x01, 0x02, 0x03, 0x04,
                       0x05, 0x06, 0x07, 0x08, 0x09, 0x0a};
  const char data2[] = {0xda, 0xab, 0x05, 0x80, 0x0a, 0x00, 0x00, 0x00,
                        0x7b, 0x57, 0x20, 0x25,
                        0x00, 0x00, 0xa1, 0x52, 0x01, 0x02, 0x03, 0x04,
                        0x05, 0x06, 0x07, 0x08, 0x09, 0x0a};
  d.Load(data2, sizeof(data2));
  d.PayloadCorrectness();

  const char test_payload[] = {0x01, 0x02, 0x03, 0x04, 0x05,
                               0x06, 0x07, 0x08, 0x09, 0x0a};

  dp.SetVersion(5);
  dp.SetPayloadChecksum(true);
  dp.SetPayload(test_payload, sizeof(test_payload));
  auto test_datagram = dp.Get();

  try {
    asio::io_context context;
    upzp::client_com::ClientCommunication client_comm(context, 2137);
    client_comm.Start();
    context.run();  
  } catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }

  while (true);
}
