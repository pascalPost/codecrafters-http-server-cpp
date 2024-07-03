#include "../include/server.h"

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
    server.add_endpoint("/echo/{str}", [](const auto &matches) -> std::string {
        const std::string str = matches.at("str");
        return std::format("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {}\r\n\r\n{}", str.size(),
                           str);
    });
    server.run();

    return EXIT_SUCCESS;
}
