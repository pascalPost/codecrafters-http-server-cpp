#include <filesystem>

#include "../include/server.h"
#include "../include/http.h"

#include <iostream>
#include <string>
#include <format>
#include <fstream>

#include "../include/config.h"

int main(const int args, const char *const argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "Logs from your program will appear here!\n";

    const Config config = parse(args, argv);

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

    if (config.directory()) {
        std::filesystem::path dir{*config.directory()};
        server.add_endpoint("/files/{filename}", [&dir](const auto &data) -> std::string {
            const auto filename = data.url_pattern().get("filename").value_or("");
            const auto file = dir.append(filename);

            if (!std::filesystem::exists(file)) {
                return "HTTP/1.1 404 Not Found\r\n\r\n";
            }

            std::ifstream is(file, std::ios::binary);
            const std::vector<char> file_content((std::istreambuf_iterator<char>(is)),
                                                 std::istreambuf_iterator<char>());

            return std::format(
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/octet-stream\r\n"
                "Content-Length: {}\r\n"
                "\r\n"
                "{}", file_content.size(), file_content.data());
        });
    }

    server.run();

    return EXIT_SUCCESS;
}
