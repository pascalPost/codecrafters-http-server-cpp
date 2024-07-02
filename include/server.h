#pragma once

#include <functional>
#include <map>
#include <memory>

#include "socket.h"

namespace http_server {
    class Socket;

    class Server {
    public:
        explicit Server(unsigned port, int connection_backlog = 5);

        void add_endpoint(std::string &&path, std::function<std::string ()> &&f);

        void start() const;

        void set_not_found_message(std::string &&message);

    private:
        std::unique_ptr<Socket> socket;
        std::map<std::string, std::function<std::string()> > endpoints;
        std::string not_found_response{"HTTP/1.1 404 Not Found\r\n\r\n"};
    };
}
