#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include <iostream>
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

int main() { 
  asio::error_code err;
  asio::io_context context;

  asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", err), 80);
  asio::ip::tcp::socket socket(context);
  socket.connect(endpoint, err);
  if (!err) {
    std::cout << "Connected!\n";
  } else {
    std::cout << "Failed to connect to address:\n"
              << err.message() << std::endl;
  }

  if (socket.is_open()) {
    std::string request =
        "GET /index.html HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Connection: close\r\n\r\n";

    socket.write_some(asio::buffer(request.data(), request.size()), err);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(200ms);

    size_t bytes = socket.available();
    if (bytes > 0) {
      std::vector<char> buffer(bytes);
      socket.read_some(asio::buffer(buffer.data(), buffer.size()), err);

      for (auto c : buffer) std::cout << c;
    }
  }

  return 0; 
}