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
    server.add_endpoint("/", [](const auto &) -> std::string { return "HTTP/1.1 200 OK\r\n\r\n"; });
    server.add_endpoint("/echo/{str}", [](const auto &matches) -> std::string {
        const auto str = matches.at("str");
        return std::format("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {}\r\n\r\n{}", str.size(),
                           str);
    });
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

TEST_CASE("The http server is able to match url pattern and use them in a response") {
    auto server_process = std::async(server_setup);

    auto response = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET /echo/abc HTTP/1.1\r\n\r\n");
    });

    response.wait();
    const std::string response_message = response.get();
    REQUIRE(response_message.starts_with("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 3\r\n\r\nabc"))
    ;
}
