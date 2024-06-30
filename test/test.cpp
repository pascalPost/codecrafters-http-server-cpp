#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <future>

#include "../include/helper.h"
#include "../include/server.h"

TEST_CASE("The http server is able to answer a http request from a client") {
    auto a1 = std::async([]() {
        constexpr int port{4221};
        const http_server::Server server(port, 5);
        const std::string message{"HTTP/1.1 200 OK\r\n\r\n"};
        server.accept(message);
    });

    auto response = std::async([]() -> std::string {
        sleep(2);

        /* first what are we going to send and where are we going to send it? */
        int portno = 4221;
        char *host = "localhost";
        const std::string message{"GET / HTTP/1.1\r\n\r\n"};

        struct hostent *server;
        struct sockaddr_in serv_addr;
        int sockfd, sent, received, total;
        char response[4096];

        printf("Request:\n%s\n", message.c_str());

        /* create the socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) throw std::runtime_error("ERROR opening socket");

        /* lookup the ip address */
        server = gethostbyname(host);
        if (server == NULL) throw std::runtime_error("ERROR no such host");

        /* fill in the structure */
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

        /* connect the socket */
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            throw std::runtime_error("ERROR connecting");

        /* send the request */
        if (write(sockfd, message.c_str(), message.length()) < 0)
            throw std::runtime_error("ERROR writing message to socket");
        // total = strlen(message);
        // sent = 0;
        // do {
        //     bytes = write(sockfd, message + sent, total - sent);
        //     if (bytes < 0)
        //         throw std::runtime_error("ERROR writing message to socket");
        //     if (bytes == 0)
        //         break;
        //     sent += bytes;
        // } while (sent < total);

        /* receive the response */
        memset(response, 0, sizeof(response));
        total = sizeof(response) - 1;
        received = 0;
        do {
            int bytes = read(sockfd, response + received, total - received);
            if (bytes < 0)
                throw std::runtime_error(std::format("ERROR reading response from socket: {}",
                                                     http_server::get_error_description()));
            if (bytes == 0)
                break;
            received += bytes;
        } while (received < total);

        /*
         * if the number of received bytes is the total size of the
         * array then we have run out of space to store the response
         * and it hasn't all arrived yet - so that's a bad thing
         */
        if (received == total)
            throw std::runtime_error("ERROR storing complete response from socket");

        /* close the socket */
        close(sockfd);

        /* process response */
        printf("Response:\n%s\n", response);

        return {response};
    });

    response.wait();
    const std::string response_message = response.get();

    REQUIRE(response_message.starts_with("HTTP/1.1 200 OK"));
}
