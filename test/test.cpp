#include <catch2/catch_test_macros.hpp>

#include <unistd.h>
#include <future>
#include <thread>

#include "../include/helper.h"
#include "../include/server.h"
#include "../include/http.h"

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
    server.add_endpoint("/echo/{str}", [](const auto &data) -> std::string {
        const auto &str_url_pattern = data.url_pattern().get("str").value();
        return std::format(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}", str_url_pattern.size(),
            str_url_pattern);
    });
    server.add_endpoint("/user-agent", [](const auto &data) -> std::string {
        const auto &agent = data.request().headers().field_value("User-Agent").value();
        return std::format(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}", agent.size(),
            agent);
    });
    server.accept();
};

TEST_CASE("The http server is able to answer a http request from a client") {
    auto server_process = std::async(server_setup);

    auto client_process = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET / HTTP/1.1\r\n\r\n");
    });

    client_process.wait();
    const std::string response_message = client_process.get();
    REQUIRE(response_message.starts_with("HTTP/1.1 200 OK"));
}

TEST_CASE("The http server sends a 404 on an unknown route") {
    auto server_process = std::async(server_setup);

    auto client_process = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET /test HTTP/1.1\r\n\r\n");
    });

    client_process.wait();
    const std::string response_message = client_process.get();
    REQUIRE(response_message.starts_with("HTTP/1.1 404 Not Found"));
}

TEST_CASE("The http server is able to match url pattern and use them in a response") {
    auto server_process = std::async(server_setup);

    auto client_process = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET /echo/abc HTTP/1.1\r\n\r\n");
    });

    client_process.wait();
    const std::string response_message = client_process.get();
    REQUIRE(response_message.starts_with("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 3\r\n\r\nabc"))
    ;
}

TEST_CASE("The http server is able to read a request header and use its content in a response") {
    auto server_process = std::async(server_setup);

    auto client_process = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port,
                    "GET /user-agent HTTP/1.1\r\n"
                    "Host: localhost:4221\r\n"
                    "User-Agent: foobar/1.2.3\r\n"
                    "Accept: */*\r\n"
                    "\r\n");
    });

    client_process.wait();
    const std::string response_message = client_process.get();
    REQUIRE(response_message.starts_with("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 3\r\n\r\nabc"))
    ;
}
