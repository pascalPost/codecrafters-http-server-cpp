#pragma once

#include <string>

namespace http_server {

class Socket {
public:
  Socket();
  explicit Socket(int file_descriptor);
  ~Socket();
  void set_options() const;
  void bind(unsigned port) const;
  void set_listen(int connection_backlog = 5) const;
  [[nodiscard]] Socket accept() const;
  void write(const std::string& message) const;

private:
  int file_descriptor;
};

} // namespace http_server
