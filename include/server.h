#pragma once

#include <functional>
#include <memory>
#include "../include/socket.h"

namespace http_server {
    class Endpoints;
    class Endpoint_input;

    /// Server provides functions to allow http communication.
    class Server {
    public:
        /// Server is constructed given a port and optionally a connection backlog.
        explicit Server(unsigned port, int connection_backlog = 5);

        /// add_endpoint allows to specify a function to be called on the given path and http method.
        void add_endpoint(
            std::string_view method,
            std::string_view path,
            std::function<std::string(const Endpoint_input &)> &&f) const;

        /// run the server starts a non returning loop accepting and responing to incoming requests.
        [[noreturn]] void run() const;

        /// set_not_found_message allows to custmize the 404 response message.
        void set_not_found_message(std::string &&message);

        /// accept incoming connections on the listening socket and reutrn a new connected socket.
        [[nodiscard]] Socket accept() const;

        /// respond on the given connected socket; this method can be run in a thread started after accept.
        void respond(Socket &&) const;

    private:
        Socket socket;
        std::unique_ptr<Endpoints> endpoints;
        std::string not_found_response{"HTTP/1.1 404 Not Found\r\n\r\n"};
    };
}
