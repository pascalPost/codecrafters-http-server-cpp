#include "../include/server.h"

#include <iostream>
#include "../include/log.h"

namespace http_server {
    Server::Server(const unsigned port, const int connection_backlog)
        : socket{std::make_unique<Socket>()} {
        socket->set_options();
        socket->bind(port);
        socket->set_listen(connection_backlog);
    }

    void Server::accept(const std::string &message) const {
        log<log_level::INFO>("Waiting for a client to connect...");
        const auto connection = socket->accept();
        log<log_level::INFO>("Client connected");

        const auto request_message = connection.read();
        log<log_level::DEBUG>("client request:\n{}", request_message);

        log<log_level::DEBUG>("sending response:\n{}", message);
        connection.write(message);
    }
} // namespace http_server
