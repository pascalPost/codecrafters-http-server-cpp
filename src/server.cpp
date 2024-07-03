#include "../include/server.h"
#include "../include/http.h"
#include "../include/log.h"

namespace http_server {
    Server::Server(const unsigned port, const int connection_backlog)
        : socket{std::make_unique<Socket>()} {
        socket->set_options();
        socket->bind(port);
        socket->set_listen(connection_backlog);
    }

    void Server::add_endpoint(std::string &&path,
                              std::function<std::string(std::unordered_map<std::string, std::string>)> &&f) {
        endpoints.try_emplace(Url{path}, f);
    }

    void Server::accept() const {
        log<log_level::INFO>("Waiting for a client to connect...");
        const auto connection = socket->accept();
        log<log_level::INFO>("Client connected");

        auto request_message = connection.read();
        log<log_level::DEBUG>("client request:\n{}", request_message);

        const http::messages::Request request{std::move(request_message)};
        const auto request_line = request.request_line();
        const auto target = request_line.request_target();

        std::string response_message = not_found_response;
        for (const auto &[url, func]: endpoints) {
            if (auto res = Url{url}.match(target); res) {
                response_message = func(*res);
            }
        }

        log<log_level::DEBUG>("sending response:\n{}", response_message);
        connection.write(response_message);
    }

    void Server::run() const {
        while (true) {
            accept();
        }
    }


    void Server::set_not_found_message(std::string &&message) {
        not_found_response = std::move(message);
    }
} // namespace http_server
