#pragma once

#include "socket.h"

#include <memory>

namespace http_server {

class Socket;

class Server {
public:
  explicit Server(unsigned port, int connection_backlog = 5);
  void accept(const std::string& message) const;
private:
  std::unique_ptr<Socket> socket;
};

}
