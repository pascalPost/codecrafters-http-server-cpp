#pragma once

#include <optional>
#include <string>

namespace http_server {
    /// Socket represents an endpoint for communication and is a wrapper around the c socket handling.
    class Socket {
    public:
        /// Socket returns a new connection based socket.
        Socket();

        /// Socket returns a new connection based socket with the given file descriptor handle.
        explicit Socket(int file_descriptor);

        /// ~Socket destructs a socket closing an active socket based on the file descriptor handle.
        ~Socket();

        /// Socket is not allowed to be copy constructed.
        Socket(const Socket &) = delete;

        /// Socket is not allowed to be copy assigned.
        Socket &operator()(const Socket &) = delete;

        /// Socket is move-constructable.
        Socket(Socket &&) noexcept;

        /// Socket is move-assignable.
        Socket &operator()(Socket &&) noexcept;

        /// set_options modifies options to prevent 'Address already in use' errors.
        void set_options() const;

        /// bind the socket to the given port.
        void bind(unsigned port) const;

        /// connect the socket to the given hostname and port.
        void connect(const std::string &hostname, unsigned port) const;


        /// set_listen marks the socket as passive and allows to accept incoming connections.
        void set_listen(int connection_backlog = 5) const;

        /// accept a connection and return a new Socket for the communication.
        [[nodiscard]] Socket accept() const;

        /// write the given message to the socket.
        void write(const std::string &message) const;

        /// read an incoming message from the socket and return it as a string.
        [[nodiscard]] std::string read() const;

    private:
        /// file_descriptor is optional to allow indicationg stale, moved-from handles that do not need to be closed.
        std::optional<int> file_descriptor;
    };
} // namespace http_server
