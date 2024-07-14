#include "../include/server.h"

#include <thread>

#include "../include/http.h"
#include "../include/log.h"
#include "../include/endpoint-input.h"
#include "../include/socket.h"
#include "../include/endpoints.h"

namespace http_server {
    Server::Server(const unsigned port, const int connection_backlog) : endpoints{std::make_unique<Endpoints>()} {
        socket.set_options();
        socket.bind(port);
        socket.set_listen(connection_backlog);
    }

    void Server::add_endpoint(
        const std::string_view method,
        const std::string_view path,
        std::function<std::string(const Endpoint_input &)> &&f) const {
        endpoints->add(method, path, std::move(f));
    }

    Socket Server::accept() const {
        log<log_level::INFO>("Waiting for a client to connect...");
        Socket connection = socket.accept();
        log<log_level::INFO>("Client connected");
        return connection;
    }

    void Server::respond(Socket &&connection) const {
        std::string request_message = connection.read();
        log<log_level::DEBUG>("client request:\n{}", request_message);

        const http::messages::Request request{std::move(request_message)};
        const auto request_line = request.request_line();
        const auto method = request_line.http_method();
        const auto target = request_line.request_target();

        std::string response_message = endpoints->run(method, target, request).value_or(not_found_response);

        log<log_level::DEBUG>("sending response:\n{}", response_message);
        connection.write(response_message);
    }


    void Server::run() const {
        // the used threading solution is a placeholder for a threadpool solution.
        while (true) {
            auto connection = socket.accept();
            std::thread th{
                [&, connection = std::move(connection)] mutable {
                    this->respond(std::move(connection));
                }
            };
            th.detach();
        }
    }


    void Server::set_not_found_message(std::string &&message) {
        not_found_response = std::move(message);
    }
} // namespace http_server
