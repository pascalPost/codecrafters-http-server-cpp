#include "catch2/catch_test_macros.hpp"

#include "../include/http.h"
#include "../include/server.h"

TEST_CASE("http request parsing") {
    std::string http_message{
        "GET /index.html HTTP/1.1\r\nHost: localhost:4221\r\nUser-Agent: curl/7.64.1\r\nAccept: */*\r\n\r\n"
    };

    http_server::http::messages::Request request{std::move(http_message)};
    const auto request_line = request.request_line();

    REQUIRE(request_line.line() == "GET /index.html HTTP/1.1");
    REQUIRE(request_line.http_method() == http_server::http::messages::Method::GET);
    REQUIRE(request_line.request_target() == "/index.html");
    REQUIRE(request_line.http_version() == "HTTP/1.1");

    REQUIRE(request.headers().content() == "Host: localhost:4221\r\nUser-Agent: curl/7.64.1\r\nAccept: */*");
    REQUIRE(request.headers().field_value("Host").value() == "localhost:4221");
    REQUIRE(request.headers().field_value("User-Agent").value() == "curl/7.64.1");
    REQUIRE(request.headers().field_value("Accept").value() == "*/*");
    REQUIRE(request.body().empty());
}

TEST_CASE("http methods conversion") {
    using http_server::http::messages::Method;
    using http_server::http::messages::from_string;
    REQUIRE(from_string("GET") == Method::GET);
    REQUIRE(from_string("HEAD") == Method::HEAD);
    REQUIRE(from_string("POST") == Method::POST);
}
