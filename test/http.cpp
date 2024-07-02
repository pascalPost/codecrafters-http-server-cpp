#include "../include/http.h"

#include "../include/server.h"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("http request parsing") {
    std::string http_message{
        "GET /index.html HTTP/1.1\r\nHost: localhost:4221\r\nUser-Agent: curl/7.64.1\r\nAccept: */*\r\n\r\n"
    };

    http_server::http::messages::Request request{std::move(http_message)};
    const auto request_line = request.request_line();

    REQUIRE(request_line.line() == "GET /index.html HTTP/1.1");
    REQUIRE(request_line.http_method() == "GET");
    REQUIRE(request_line.request_target() == "/index.html");
    REQUIRE(request_line.http_version() == "HTTP/1.1");

    REQUIRE(request.headers() == "Host: localhost:4221\r\nUser-Agent: curl/7.64.1\r\nAccept: */*");
    REQUIRE(request.body().empty());
}
