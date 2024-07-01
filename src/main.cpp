#include "../include/server.h"

#include <iostream>
#include <string>

int main(int, char **) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "Logs from your program will appear here!\n";

    constexpr int port{4221};
    const http_server::Server server(port);

    server.accept();

    return EXIT_SUCCESS;
}
