#pragma once

#include <functional>
#include <unordered_map>
#include <memory>

#include "endpoint-input.h"
#include "socket.h"
#include "url.h"

namespace http_server {
    class Socket;
    class Endpoint_input;

    class Server {
    public:
        explicit Server(unsigned port, int connection_backlog = 5);

        void add_endpoint(std::string &&path,
                          std::function<std::string(const Endpoint_input &)> &&f);

        void accept() const;

        [[noreturn]] void run() const;

        void set_not_found_message(std::string &&message);

    private
    :
        std::unique_ptr<Socket> socket;
        std::unordered_map<Url, std::function<std::string(const Endpoint_input &)> > endpoints;
        std::string not_found_response{"HTTP/1.1 404 Not Found\r\n\r\n"};
    };
}
