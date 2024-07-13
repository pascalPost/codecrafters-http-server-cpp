#include <catch2/catch_test_macros.hpp>

#include <unistd.h>
#include <future>
#include <thread>
#include <filesystem>
#include <fstream>

#include "../include/server.h"
#include "../include/http.h"

std::string send(const std::string &address, const unsigned port, const std::string &message) {
    const http_server::Socket socket{};
    socket.connect(address, port);
    socket.write(message);
    return socket.read();
}

constexpr int port{4221};

const auto server_setup = []() {
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

    const std::filesystem::path source_file_path{__FILE__};
    const auto dir = source_file_path.parent_path() / "data";
    server.add_endpoint("/files/{filename}", [&dir](const auto &data) -> std::string {
        const auto filename = data.url_pattern().get("filename").value();
        const auto file = dir / filename;

        if (!std::filesystem::exists(file)) {
            return "HTTP/1.1 404 Not Found\r\n\r\n";
        }

        std::ifstream is(file, std::ios::binary);
        const std::vector<char> file_content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

        return std::format(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/octet-stream\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}", file_content.size(), file_content.data());
    });
    server.respond(server.accept());
};

TEST_CASE("The http server is able to answer a http request from a client") {
    auto server_process = std::async(server_setup);

    auto client_process = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET / HTTP/1.1\r\n\r\n");
    });

    client_process.wait();
    const std::string response_message = client_process.get();
    REQUIRE(response_message == "HTTP/1.1 200 OK\r\n\r\n");
}

TEST_CASE("The http server sends a 404 on an unknown route") {
    auto server_process = std::async(server_setup);

    auto client_process = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET /test HTTP/1.1\r\n\r\n");
    });

    client_process.wait();
    const std::string response_message = client_process.get();
    REQUIRE(response_message == "HTTP/1.1 404 Not Found\r\n\r\n");
}

TEST_CASE("The http server is able to match url pattern and use them in a response") {
    auto server_process = std::async(server_setup);

    auto client_process = std::async([]() -> std::string {
        sleep(2);
        return send("localhost", port, "GET /echo/abc HTTP/1.1\r\n\r\n");
    });

    client_process.wait();
    const std::string response_message = client_process.get();
    REQUIRE(response_message == "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 3\r\n\r\nabc")
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
    REQUIRE(response_message == "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nfoobar/1.2.3")
    ;
}

TEST_CASE("The http server is able to share files") {
    SECTION("existing file") {
        auto server_process = std::async(server_setup);

        auto client_process = std::async([]() -> std::string {
            sleep(2);
            return send("localhost", port, "GET /files/test HTTP/1.1\r\n\r\n");
        });

        client_process.wait();
        const std::string response_message = client_process.get();
        REQUIRE(
            response_message ==
            "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: 13\r\n\r\nHello, World!")
        ;
    }

    SECTION("non-existing file") {
        auto server_process = std::async(server_setup);

        auto client_process = std::async([]() -> std::string {
            sleep(2);
            return send("localhost", port, "GET /files/non-existing-file HTTP/1.1\r\n\r\n");
        });

        client_process.wait();
        const std::string response_message = client_process.get();
        REQUIRE(response_message == "HTTP/1.1 404 Not Found\r\n\r\n")
        ;
    }
}
