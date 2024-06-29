#include "../include/server.h"

#include <iostream>
#include <string>

int main(int, char **) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "Logs from your program will appear here!\n";

  constexpr int port {4221};
  const http_server::Server server (port);

  const std::string message {"HTTP/1.1 200 OK\r\n\r\n"};

  server.accept(message);

  return EXIT_SUCCESS;
}
