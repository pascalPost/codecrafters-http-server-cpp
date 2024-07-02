#include <catch2/catch_test_macros.hpp>

#include <unistd.h>
#include <future>
#include <thread>

#include "../include/helper.h"
#include "../include/server.h"

std::string send(const std::string &address, const unsigned port, const std::string &message) {
    const http_server::Socket socket{};
    socket.connect(address, port);
    socket.write(message);
    return socket.read();
}

constexpr int port{4221};

auto server_setup = []() {
    http_server::Server server(port, 5);
    server.add_endpoint("/", []() -> std::string { return "HTTP/1.1 200 OK\r\n\r\n"; });
    server.accept();
};

TEST_CASE("The http server is able to answer a http request from a client") {
    auto server_process = std::async(server_setup);

    auto response = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET / HTTP/1.1\r\n\r\n");
    });

    response.wait();
    const std::string response_message = response.get();
    REQUIRE(response_message.starts_with("HTTP/1.1 200 OK"));
}

TEST_CASE("The http server sends a 404 on an unknown route") {
    auto server_process = std::async(server_setup);

    auto response = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET /test HTTP/1.1\r\n\r\n");
    });

    response.wait();
    const std::string response_message = response.get();
    REQUIRE(response_message.starts_with("HTTP/1.1 404 Not Found"));
}
