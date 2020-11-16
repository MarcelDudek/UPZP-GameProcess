#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include <iostream>

int main() {
  std::cout << "Hello world!";
  return 0;
}
