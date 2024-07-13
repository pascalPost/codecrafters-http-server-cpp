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

    /// Server provides functions to allow http communication.
    class Server {
    public:
        /// Server is constructed given a port and optionally a connection backlog.
        explicit Server(unsigned port, int connection_backlog = 5);

        /// add_endpoint allows to specify a function to be called on the given path.
        void add_endpoint(std::string &&path,
                          std::function<std::string(const Endpoint_input &)> &&f);

        /// run the server starts a non returning loop accepting and responing to incoming requests.
        [[noreturn]] void run() const;

        /// set_not_found_message allows to custmize the 404 response message.
        void set_not_found_message(std::string &&message);

        /// accept incoming connections on the listening socket and reutrn a new connected socket.
        [[nodiscard]] Socket accept() const;

        /// respond on the given connected socket; this method can be run in a thread started after accept.
        void respond(Socket &&) const;

    private:
        std::unique_ptr<Socket> socket;
        std::unordered_map<Url, std::function<std::string(const Endpoint_input &)> > endpoints;
        std::string not_found_response{"HTTP/1.1 404 Not Found\r\n\r\n"};
    };
}
