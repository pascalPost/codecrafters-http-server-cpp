#include "../include/server.h"
#include "../include/http.h"

#include <iostream>
#include <string>
#include <format>

int main(int, char **) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "Logs from your program will appear here!\n";

    constexpr int port{4221};
    http_server::Server server(port);
    server.add_endpoint("/", [](const auto &) -> std::string { return "HTTP/1.1 200 OK\r\n\r\n"; });
    server.add_endpoint("/echo/{str}", [](const auto &data) -> std::string {
        const auto str = data.url_pattern().get("str").value_or("");
        return std::format(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}", str.size(),
            str);
    });
    server.add_endpoint("/user-agent", [](const auto &data) -> std::string {
        const auto &agent = data.request().headers().field_value("User-Agent").value_or("");
        return std::format(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}", agent.size(),
            agent);
    });
    server.run();

    return EXIT_SUCCESS;
}
