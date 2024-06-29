#include "../include/server.h"

#include <iostream>

namespace http_server {

Server::Server(const unsigned port, const int connection_backlog) : socket{} {
  socket->set_options();
  socket->bind(port);
  socket->set_listen(connection_backlog);
}

void Server::accept(const std::string& message) const {
  std::cout << "Waiting for a client to connect...\n";
  const auto connection = socket->accept();
  std::cout << "Client connected\n";
  connection.write(message);
}

} // namespace http_server
