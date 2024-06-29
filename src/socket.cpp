#include "../include/socket.h"
#include "../include/helper.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

namespace http_server {

void Socket::set_options() const {
  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  const int reuse = 1;
  if (setsockopt(file_descriptor, SOL_SOCKET, SO_REUSEADDR, &reuse,
                 sizeof(reuse)) < 0) {
    throw std::runtime_error("Error in setsockopt: " + get_error_description());
  }
}

void Socket::bind(const unsigned port) const {
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(narrow_cast<in_port_t>(port));

  if (::bind(file_descriptor, reinterpret_cast<const sockaddr *>(&address),
             sizeof(address)) != 0) {
    throw std::runtime_error(std::format("Faild to bind to port {} : {}", port,
                                         http_server::get_error_description()));
  }
}

void Socket::set_listen(const int connection_backlog) const {
  // marks the socket as a passive socket, that is, as a socket that will be
  // used to accept incoming connection requests using accept
  if (listen(file_descriptor, connection_backlog) != 0) {
    throw std::runtime_error(
        std::format("Error in listen: {}", get_error_description()));
  }
}

Socket::Socket() : file_descriptor{} {
  file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (file_descriptor < 0) {
    throw std::runtime_error("Error in server socket creation:" +
                             get_error_description());
  }
}

Socket::Socket(const int file_descriptor) : file_descriptor{file_descriptor} {
  if (file_descriptor < 0) {
    throw std::runtime_error("Error in server socket creation:" +
                             get_error_description());
  }
}

Socket::~Socket() {
  if (close(file_descriptor) < 0) {
    std::cerr << "Error in socket close: " + get_error_description();
  }
}

Socket Socket::accept() const {
  sockaddr_in client_addr{};
  int client_addr_len = sizeof(client_addr);

  const auto connected_fd = ::accept(
      file_descriptor, reinterpret_cast<sockaddr *>(&client_addr),
      reinterpret_cast<socklen_t *>(&client_addr_len));

  return Socket{connected_fd};
}

void Socket::write(const std::string &message) const {
  if (::write(file_descriptor, message.c_str(), message.size()) < 0) {
    throw std::runtime_error("Error in writung response: " +
                             std::string{strerror(errno)});
  }
}

} // namespace http_server