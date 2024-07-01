#include <catch2/catch_test_macros.hpp>

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <future>

#include "../include/helper.h"
#include "../include/server.h"

std::string send(const std::string &address, const unsigned port, const std::string &message) {
    http_server::Socket socket{};
    socket.connect(address, port);
    socket.write(message);
    return socket.read();
}

TEST_CASE("The http server is able to answer a http request from a client") {
    constexpr int port{4221};

    auto server = std::async([]() {
        const http_server::Server server(port, 5);
        server.accept();
    });

    auto response = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET / HTTP/1.1\r\n\r\n");
    });

    response.wait();
    const std::string response_message = response.get();

    REQUIRE(response_message.starts_with("HTTP/1.1 200 OK"));
}

TEST_CASE("The http server sends a 404 on an unknown route") {
    constexpr int port{4221};

    auto server = std::async([]() {
        const http_server::Server server(port, 5);
        server.accept();
    });

    auto response = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET /test HTTP/1.1\r\n\r\n");
    });

    response.wait();
    const std::string response_message = response.get();

    REQUIRE(response_message.starts_with("HTTP/1.1 404 Not Found"));
}
