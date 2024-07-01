#include "../include/server.h"

#include <iostream>

#include "../include/http.h"
#include "../include/log.h"

namespace http_server {
    Server::Server(const unsigned port, const int connection_backlog)
        : socket{std::make_unique<Socket>()} {
        socket->set_options();
        socket->bind(port);
        socket->set_listen(connection_backlog);
    }


    // TODO create proper class and use as a type that is returned from read and also contains the message as a std::string
    struct http_message {
        std::string_view headers;
        std::string_view body;
    };

    [[nodiscard]] http_message split_http_message(const std::string_view message) {
        const auto header_body_seperation_position = message.find("\r\n\r\n");
        if (header_body_seperation_position == std::string_view::npos) {
            throw std::runtime_error("Invalid HTTP message: no header-body separator found");
        }

        const std::string_view headers = message.substr(0, header_body_seperation_position);
        const std::string_view body = message.substr(header_body_seperation_position + 4);

        return {headers, body};
    }

    void Server::accept() const {
        log<log_level::INFO>("Waiting for a client to connect...");
        const auto connection = socket->accept();
        log<log_level::INFO>("Client connected");

        auto request_message = connection.read();
        log<log_level::DEBUG>("client request:\n{}", request_message);

        http::messages::Request request{std::move(request_message)};
        const auto request_line = request.request_line();
        const auto target = request_line.request_target();

        std::string message{};
        if (target == "/") {
            message = "HTTP/1.1 200 OK\r\n\r\n";
        } else {
            message = "HTTP/1.1 404 Not Found\r\n\r\n";
        }


        log<log_level::DEBUG>("sending response:\n{}", message);
        connection.write(message);
    }
} // namespace http_server
